#include <ctype.h>
#include "die.h"
#include "value.h"
#include "basic.h"
#include "buf.h"
#include "long.h"
#include "memory.h"
#include "parse.h"
#include "run.h"
#include "string.h"
#include "sym.h"

/* LATER 20120920 get rid of the symbols stacks.  */

/* LATER 20120920 instead of returning 0 on syntax error, return
syntax_error(msg,line) */

static long line;     /* current line number */
static char *error;   /* syntax error if any */

static value C;
static value I;
static value Y;
static value lam;

static value parse_exp(void);
static value lambda(value, value);

/*
Grammar:

exp  =>  empty
exp  =>  term exp
exp  =>  ; exp
exp  =>  \ sym def exp

term =>  sym
term =>  ( exp )

def  =>  empty
def  =>  is term

is   =>  =
is   =>  ==

sym  => name
sym  => string

string => simple_string
string => complex_string

The Fexl parser reads an expression from the input until it reaches -1 (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.
*/

/* The read_ch function reads the next character.  It can be set to read from
any source such as a file or a string. */
int (*read_ch)(void);

static int ch = 0;      /* current character */

static void next_ch(void)
	{
	ch = read_ch();
	if (ch == '\n') line++;
	}

static void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == -1)
			{
			next_ch();
			return;
			}
		else
			next_ch();
		}
	}

static int at_white(void)
	{
	return isspace(ch) || ch == 0;
	}

static void skip_white(void)
	{
	while (1)
		{
		if (!at_white() || ch == -1)
			return;
		else
			next_ch();
		}
	}

static void skip_filler(void)
	{
	while (1)
		{
		if (ch == '#')
			skip_line();
		else if (at_white())
			skip_white();
		else
			return;
		}
	}

static value collect_string(char *term_string, long term_len, long first_line)
	{
	struct buf *buf = 0;
	long match_pos = 0;

	while (1)
		{
		if (ch == term_string[match_pos])
			{
			match_pos++;
			next_ch();
			}
		else if (match_pos > 0)
			{
			if (match_pos >= term_len)
				{
				long len;
				char *string = buf_clear(&buf,&len);
				return Qchars(string,len);
				}
			else
				{
				long i;
				for (i = 0; i < match_pos; i++)
					buf_add(&buf, term_string[i]);

				match_pos = 0;
				}
			}
		else if (ch == -1)
			{
			buf_discard(&buf);
			line = first_line;
			error = "Unclosed string";
			return 0;
			}
		else
			{
			buf_add(&buf, ch);
			next_ch();
			}
		}
	}

static value parse_simple_string(void)
	{
	long first_line = line;
	next_ch();
	return collect_string("\"", 1, first_line);
	}

static value parse_complex_string(void)
	{
	long first_line = line;
	struct buf *term = 0;

	while (1)
		{
		if (at_white())
			{
			long term_len;
			char *term_string = buf_clear(&term,&term_len);
			next_ch();

			value str = collect_string(term_string, term_len, first_line);
			free_memory(term_string, term_len+1);
			return str;
			}
		else if (ch == -1)
			{
			line = first_line;
			error = "Unclosed string terminator";
			buf_discard(&term);
			return 0;
			}
		else
			{
			buf_add(&term, ch);
			next_ch();
			}
		}
	}

/*
A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest possible rule that
can work.
*/
static value parse_name(int allow_eq)
	{
	struct buf *buf = 0;
	while (1)
		{
		if (
			at_white()
			|| ch == '\\'
			|| ch == '('
			|| ch == ')'
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| (ch == '=' && !allow_eq)
			|| ch == -1
			)
			{
			long len;
			char *string = buf_clear(&buf,&len);
			return Qname(string,len);
			}
		else
			{
			buf_add(&buf, ch);
			next_ch();
			}
		}
	}

/* Parse a symbol. */
static value parse_sym(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

static value find_sym(value list, value name)
	{
	value pos = list;
	while (1)
		{
		if (pos == 0)
			return 0;
		else if (sym_eq(name,pos->L))
			return pos->L;
		else
			pos = pos->R;
		}
	}

/* Symbols defined with lambda forms inside the source text */
static value inner_syms = 0;
/* Symbols defined outside the source text */
static value outer_syms = 0;
/* Line numbers of outer symbols */
static value outer_places = 0;

/* Set a value reference to a new value. */
static void set(value *pos, value val)
	{
	hold(val);
	value old = *pos;
	if (old) drop(old);
	*pos = val;
	}

/* Push a value on the stack. */
static void push(value *stack, value f)
	{
	set(stack, A(f,*stack));
	}

/* Pop a value from the stack. */
static void pop(value *stack)
	{
	set(stack, (*stack)->R);
	}

/* Return the originally encountered occurrence of a symbol.  This unifies
multiple occurrences into a single one. */
static value orig_sym(value sym, long first_line)
	{
	hold(sym);
	value orig = find_sym(inner_syms,sym);

	if (!orig)
		{
		orig = find_sym(outer_syms,sym);

		if (!orig)
			{
			/* new outer sym encountered */
			orig = sym;
			push(&outer_syms, orig);
			push(&outer_places, Qlong(first_line));
			}
		}

	drop(sym);
	return orig;
	}

static value parse_term(void)
	{
	long first_line = line;

	if (ch == '(')
		{
		next_ch();
		value exp = parse_exp();
		if (exp == 0)
			return exp;
		else if (ch == ')')
			{
			next_ch();
			return exp;
			}
		else
			{
			hold(exp);
			line = first_line;
			error = "Unclosed parenthesis";
			drop(exp);
			return 0;
			}
		}
	else
		{
		value exp = parse_sym(1);
		if (exp == 0)
			return exp;
		else if (exp->T == type_name && string_len(exp) == 0)
			{
			hold(exp);
			line = first_line;
			error = "Missing definition";
			drop(exp);
			return 0;
			}
		else
			return orig_sym(exp,first_line);
		}
	}

static void type_open(value f) { type_error(); }

int is_open(value f)
	{
	return f->T == type_open || f->T == type_name || f->T == type_string;
	}

/* This is a wrapper around the copy routine which prevents us from copying
symbols, because we still rely on pointer comparison in get_pattern.  That will
soon change, because in a new version of the code we won't even have symbols in
expressions, and we'll compute lambda patterns in a bottom-up fashion. */

static value copy_exp(value f)
	{
	if (f->T == type_name || f->T == type_string)
		return f;
	else
		return copy(f);
	}

static value apply(value f, value x)
	{
	if (f == 0)
		{
		hold(x);
		drop(x);
		return 0;
		}
	else if (x == 0)
		{
		hold(f);
		drop(f);
		return 0;
		}
	else if (f == I)
		return x;
	else
		{
		value result = A(f,x);
		if (is_open(f) && is_open(x)) result->T = type_open;
		return result;
		}
	}

/* In the new version we won't need these "scope_" routines because we'll
compute lambda patterns in a bottom-up fashion and won't need any symbol
stacks. */

static value scope_parse_term(value sym)
	{
	push(&inner_syms,sym);
	value val = parse_term();
	pop(&inner_syms);
	return val;
	}

static value scope_parse_exp(value sym)
	{
	push(&inner_syms,sym);
	value val = parse_exp();
	pop(&inner_syms);
	return val;
	}

/*
Parse a lambda form following the initial '\' character.

An ordinary lambda symbol is terminated by white space or '=', for example:

  \x=4
  \y = 5

If you want a lambda symbol to contain '=', you must use white space after the
initial '\'.  This tells the parser that the lambda symbol is terminated by
white space only, and not '='.  For example:

  \ =   = num_eq
  \ ==  = num_eq
  \ !=  = num_ne
  \ <   = num_lt
  \ <=  = num_le
  \ >   = num_gt
  \ >=  = num_ge
*/
static value parse_lambda(void)
	{
	int allow_eq = at_white();
	skip_white();

	value sym = parse_sym(allow_eq);
	hold(sym);

	value val;

	if (sym == 0)
		val = 0;
	else if (sym->T == type_name && string_len(sym) == 0)
		{
		error = "Missing lambda symbol";
		val = 0;
		}
	else
		{
		skip_filler();

		if (ch == '=')
			{
			next_ch();
			value def;
			if (ch == '=')
				{
				/* Recursive definition */
				next_ch();
				skip_filler();
				def = apply(Y,lambda(sym, scope_parse_term(sym)));
				}
			else
				{
				/* Non-recursive definition */
				skip_filler();
				def = parse_term();
				}

			val = def ? lambda(sym, scope_parse_exp(sym)) : 0;
			val = apply(val,def);
			}
		else
			/* Normal parameter (symbol without definition) */
			val = lambda(sym, scope_parse_exp(sym));
		}

	drop(sym);
	return val;
	}

/*
If you're parsing a possibly malicious script, you should use setrlimit to
impose limits not only on stack depth, but also on total memory usage and
CPU time.

For example, if you parse an infinite stream of '(' characters, you'll see a
segmentation fault due to stack overflow.  That happens due to the default
limits on stack size, without any intervention.  But if you parse an infinite
stream of ' ' characters, it will run forever unless you impose limits on CPU
time with RLIMIT_CPU.  If you parse an infinite stream of 'a' characters, it
will use an unbounded amount of memory which could slow your machine to a crawl
until it finally reaches the very large default limits.  So you should set
RLIMIT_AS, and also RLIMIT_DATA for good measure, to limit the total amount of
memory.
*/

/* Parse an expression. */
static value parse_exp(void)
	{
	value exp = I;

	while (1)
		{
		skip_filler();

		if (ch == -1 || ch == ')')
			return exp;
		else
			{
			value val;

			if (ch == '\\')
				{
				next_ch();
				if (ch == '\\')
					{
					ch = -1; /* Two backslashes simulates end of file. */
					return exp;
					}
				else
					val = parse_lambda();
				}
			else if (ch == ';')
				{
				next_ch();
				val = parse_exp();
				}
			else
				val = parse_term();

			exp = apply(exp,val);

			if (exp == 0)
				return exp;
			}
		}
	}

/* Return a copy of f with x substituted according to pattern p. */
static value subst(value p, value f, value x)
	{
	if (p->T == reduce_C)
		return f;
	else if (p->T == reduce_I)
		return x;
	else
		return A(subst(p->L,f->L,x),subst(p->R,f->R,x));
	}

/* lambda pattern form value = subst(pattern,form,value) */
static void reduce3_lambda(value f)
	{
	replace(f, subst(f->L->L->R,f->L->R,f->R));
	}

static void reduce2_lambda(value f)
	{
	f->T = reduce3_lambda;
	}

void type_lambda(value f)
	{
	f->T = reduce2_lambda;
	}

/* Get the pattern of occurrences of symbol x in form f. */
static value get_pattern(value x, value f)
	{
	if (f == x)
		return I;
	else if (f->T && f->T != type_open)
		return C;
	else
		{
		value pl = get_pattern(x,f->L);
		value pr = get_pattern(x,f->R);

		if (pl->T == reduce_C && pr->T == reduce_C)
			return C;
		else
			return A(pl,pr);
		}
	}

/* Make a lambda form using the substitution pattern for symbol x in form f. */
static value lambda(value x, value f)
	{
	hold(f);

	value val;

	if (f == 0)
		val = 0;
	else
		{
		value p = get_pattern(x,f);
		hold(p);

		if (p->T == reduce_C)
			val = A(C,f);
		else if (p->T == reduce_I)
			val = I;
		else if (p->L->T == reduce_C && p->R->T == reduce_I)
			/* Special case for the rule:  lam (C I) (L R) = L.  This greatly
			compresses many parsed forms, saving memory and time.  We copy f->L
			because it's a part of f and we'll be dropping f.  */
			val = copy_exp(f->L);
		else
			val = apply(A(lam,p),f);

		drop(p);
		}

	drop(f);
	return val;
	}

static void beg_parse(void)
	{
	C = Q(reduce_C);
	I = Q(reduce_I);
	Y = Q(reduce_Y);
	lam = Q(type_lambda);

	hold(C);
	hold(I);
	hold(Y);
	hold(lam);
	}

static void end_parse(void)
	{
	drop(C);
	drop(I);
	drop(Y);
	drop(lam);
	}

/*
Parse the source text, returning (pair ok; pair exp; symbols).

ok is true if the source is well-formed, i.e. no syntax errors.

If ok is true, exp is the parsed expression.
If ok is false, exp is (pair error line), where error is the string describing
the syntax error, and line is the line number on which it occurred.

symbols is the list of all symbols used but not defined within the source text.
It is a list of entries (pair sym line_no), where line_no is the line number on
which the symbol first occurred.

If ok is true, then the caller can take the exp and successively apply the
definitions of each symbol in the symbols list.  The result will be the actual
executable function which can then be run with "eval" in the Fexl intepreter.
*/
value parse_source(void)
	{
	beg_parse();

	value exp;

	if (read_ch == 0)
		{
		error = "Can't open file";
		exp = 0;
		}
	else
		{
		line = 1;
		next_ch();

		exp = parse_exp();
		if (exp)
			{
			if (ch != -1)
				{
				hold(exp);
				error = "Extraneous input";
				drop(exp);
				exp = 0;
				}
			}
		}

	value symbols = C;

	while (outer_syms)
		{
		value sym = outer_syms->L;
		value place = outer_places->L;

		exp = lambda(sym,exp);
		symbols = item(pair(sym,place),symbols);

		pop(&outer_syms);
		pop(&outer_places);
		}

	/* If there was an error, use (pair error line) as the expression. */
	if (exp == 0)
		exp = pair(Qstring(error),Qlong(line));

	value ok = Q(error ? reduce_F : reduce_C);
	value result = pair(ok,pair(exp,symbols));

	line = 0;
	error = 0;

	end_parse();
	return result;
	}
