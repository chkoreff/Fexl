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

static value C;
static value I;
static value Y;
static value L;
static value R;
static value query;

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
	/* Pay attention to max_steps while parsing. */
	if (max_steps >= 0 && ++cur_steps > max_steps)
		die("Your program ran too long");

	ch = read_ch();
	if (ch == '\n') line++;
	}

static void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == -1) break;
		next_ch();
		}
	next_ch();
	}

static int at_white(void)
	{
	return isspace(ch) || ch == 0;
	}

static void skip_white(void)
	{
	while (1)
		{
		if (!at_white() || ch == -1) break;
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
			break;
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
				break;

			long i;
			for (i = 0; i < match_pos; i++)
				buf_add(&buf, term_string[i]);

			match_pos = 0;
			}
		else if (ch == -1)
			{
			line = first_line;
			error = "Unclosed string";
			break;
			}
		else
			{
			buf_add(&buf, ch);
			next_ch();
			}
		}

	if (error)
		{
		buf_discard(&buf);
		return 0;
		}

	long len;
	char *string = buf_clear(&buf,&len);
	return Qchars(string,len);
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
		if (at_white() || ch == -1) break;
		buf_add(&term, ch);
		next_ch();
		}

	if (ch == -1)
		{
		line = first_line;
		error = "Unclosed string terminator";
		}

	if (error)
		{
		buf_discard(&term);
		return 0;
		}

	long term_len;
	char *term_string = buf_clear(&term,&term_len);
	next_ch();

	value str = collect_string(term_string, term_len, first_line);
	free_memory(term_string, term_len+1);
	return str;
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
			break;

		buf_add(&buf, ch);
		next_ch();
		}

	long len;
	char *string = buf_clear(&buf,&len);
	return Qname(Qchars(string,len));
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
		if (pos == 0) return 0;
		if (sym_eq(name,pos->L)) return pos->L;
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
	if (val) val->N++;
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

	if (orig != sym)
		{
		hold(sym);
		drop(sym);
		}

	return orig;
	}

static value parse_term(void)
	{
	long first_line = line;

	value exp;

	if (ch == '(')
		{
		next_ch();
		exp = parse_exp();
		if (exp)
			{
			if (ch == ')')
				next_ch();
			else
				{
				line = first_line;
				error = "Unclosed parenthesis";
				}
			}
		}
	else
		{
		exp = parse_sym(1);
		if (exp)
			{
			if (exp->T == type_name && string_len(exp) == 0)
				{
				line = first_line;
				error = "Missing definition";
				}
			else
				exp = orig_sym(exp,first_line);
			}
		}

	if (error)
		{
		hold(exp);
		drop(exp);
		exp = 0;
		}

	return exp;
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
	if (sym == 0) return 0;

	if (sym->T == type_name && string_len(sym) == 0)
		{
		error = "Missing lambda symbol";
		hold(sym);
		drop(sym);
		return 0;
		}

	hold(sym);

	skip_filler();
	int has_def = (ch == '=');

	value val = 0;

	if (has_def)
		{
		next_ch();
		if (ch == '=')
			{
			/* Recursive definition */
			next_ch();
			skip_filler();

			push(&inner_syms,sym);
			value def = parse_term();
			if (def)
				{
				value body = parse_exp();
				if (body)
					{
					val = lambda(sym,body);
					val = A(val,A(Y,lambda(sym,def)));
					}
				}
			pop(&inner_syms);
			}
		else
			{
			/* Non-recursive definition, forces eager evaluation */
			skip_filler();

			value def = parse_term();
			if (def)
				{
				push(&inner_syms,sym);
				value body = parse_exp();
				if (body)
					{
					val = lambda(sym,body);
					val = A(A(query,def),val);
					}
				pop(&inner_syms);
				}
			}
		}
	else
		{
		/* Normal parameter (symbol without definition) */
		push(&inner_syms,sym);
		value body = parse_exp();
		if (body) val = lambda(sym,body);
		pop(&inner_syms);
		}

	drop(sym);
	return val;
	}

/*
Parse an expression.

We limit the depth here to avoid a stack overflow resulting from strange inputs
like an infinite series of left parentheses.
*/
static value parse_exp(void)
	{
	if (++cur_depth > max_depth)
		{
		error = "Your program is too deeply nested.";
		return 0;
		}

	value exp = I;

	while (1)
		{
		skip_filler();
		if (ch == -1 || ch == ')') break;

		value val;

		if (ch == '\\')
			{
			next_ch();
			if (ch == '\\')
				{
				ch = -1; /* Two backslashes simulates end of file. */
				break;
				}

			val = parse_lambda();
			}
		else if (ch == ';')
			{
			next_ch();
			val = parse_exp();
			}
		else
			val = parse_term();

		if (val == 0)
			{
			hold(exp);
			drop(exp);
			exp = 0;
			break;
			}

		exp = exp == I ? val : A(exp,val);
		}

	cur_depth--;
	return exp;
	}

static value get_pattern(value sym, value fun)
	{
	if (fun == sym) return I;
	if (!fun->L) return C;

	value fl = get_pattern(sym,fun->L);
	value fr = get_pattern(sym,fun->R);
	if (fl->T == fexl_C && fr->T == fexl_C) return C;

	return A(fl,fr);
	}

/* Return a copy of fun with val substituted according to pattern p. */
static value subst(value p, value fun, value val)
	{
	if (p->T == fexl_I) return val;
	if (p->T == fexl_C) return fun;
	return A(subst(p->L,fun->L,val),subst(p->R,fun->R,val));
	}

value fexl_lambda(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return subst(f->L->L->R,f->L->R,f->R);
	}

static value lam;

/* LATER: It seems quite plausible to generate C code to implement these lambda
substitution patterns extremely fast so we don't have to call "subst" at run-
time.  I'm sure it's trivial to generate the C code, but I'll have to research
how to compile and link that code into the running executable on the fly.

Alternatively, I might just precompile some very common patterns and recognize
them here.
*/
static value lambda(value x, value f)
	{
	return A(A(lam,get_pattern(x,f)),f);
	}

static void beg_parse(void)
	{
	C = Q(fexl_C);
	I = Q(fexl_I);
	Y = Q(fexl_Y);
	L = Q(fexl_L);
	R = Q(fexl_R);
	query = Q(fexl_query);
	lam = Q(fexl_lambda);

	hold(C);
	hold(I);
	hold(Y);
	hold(L);
	hold(R);
	hold(query);
	hold(lam);
	}

static void end_parse(void)
	{
	drop(C);
	drop(I);
	drop(Y);
	drop(L);
	drop(R);
	drop(query);
	drop(lam);
	}

value parse_source(value resolve)
	{
	hold(resolve);
	beg_parse();

	line = 1;
	next_ch();

	value exp = parse_exp();
	if (exp)
		{
		if (ch != -1)
			{
			error = "Extraneous input";
			hold(exp);
			drop(exp);
			exp = 0;
			}
		}

	/* Use a dummy expression if there was an error. */
	if (exp == 0) exp = I;

	/* Start with (\yes\no exp yes exp). */
	exp = A(A(R,C),A(A(L,I),exp));

	while (outer_syms)
		{
		value sym = outer_syms->L;
		value place = outer_places->L;

		exp = A(A(A(resolve,sym),place),lambda(sym,exp));

		pop(&outer_syms);
		pop(&outer_places);
		}

	/* Finish with (exp I F).  The F quits if there are undefined symbols. */
	exp = A(A(exp,I),A(C,I));

	/*TODO LATER return (pair ok; pair exp; outer)
	where exp is the expression if ok, or the string error otherwise
	where outer is a list of (pair sym place)
	*/

	/*TODO simplify the resolution chaining */

	end_parse();
	drop(resolve);

	if (error)
		{
		hold(exp);
		drop(exp);
		exp = 0;
		}

	return exp;
	}
