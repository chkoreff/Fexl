#include <ctype.h>
#include "value.h"
#include "basic.h"
#include "buf.h"
#include "long.h"
#include "memory.h"
#include "parse.h"
#include "run.h"
#include "string.h"
#include "sym.h"

/* LATER 20120920 instead of returning 0 on syntax error, return
syntax_error(msg,line) */

static long line;     /* current line number */
static char *error;   /* syntax error if any */

static value C;
static value I;
static value Y;
static value lam;

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

static void type_open(value f) { type_error(); }

static value make_sym(type t, struct buf **buf, long first_line)
	{
	long len;
	char *string = buf_clear(buf,&len);

	value data = Qchars(string,len);
	data->T = t;

	value sym = A(Q(t),pair(data,Qlong(first_line)));
	sym->T = type_open;
	return sym;
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
				return make_sym(type_string,&buf,first_line);
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
			value sym = collect_string(term_string, term_len, first_line);
			free_memory(term_string, term_len+1);
			return sym;
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
	long first_line = line;
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
			return make_sym(type_name,&buf,first_line);
		else
			{
			buf_add(&buf, ch);
			next_ch();
			}
		}
	}

static int is_null_name(value exp)
	{
	return exp->T == type_open
		&& exp->L->T == type_name
		&& string_len(exp->R->L) == 0;
	}

/* Parse a symbol. */
static value new_parse_sym(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

static value parse_exp(void);

static value parse_term(void)
	{
	if (ch == '(')
		{
		long first_line = line;
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
		value exp = new_parse_sym(1);

		if (exp == 0)
			return exp;
		else if (is_null_name(exp))
			{
			hold(exp);
			line = get_long(exp->R->R);
			error = "Missing definition";
			drop(exp);
			return 0;
			}
		else
			return exp;
		}
	}

/* Determine if two symbols (names or strings) are equivalent. */
static int sym_eq(value x, value y)
	{
	return x->T == type_open && y->T == type_open
		&& (x->L->T == type_name || x->L->T == type_string)
		&& x->L->T == y->L->T
		&& string_eq(x->R->L,y->R->L);
	}

/* Apply f to x, marking the result as open if both sides are open. */
static value apply(value f, value x)
	{
	value result = A(f,x);
	if (f->T == type_open || x->T == type_open)
		result->T = type_open;
	return result;
	}

/* Get the pattern of occurrences of symbol x in form f. */
static value get_pattern(value x, value f)
	{
	if (sym_eq(x,f))
		return I;
	else if (f->T != type_open)
		return C;
	else if (f->L->T == type_name || f->L->T == type_string)
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

/* LATER 20120921 unify with get_pattern into one lambda routine.  This is good
preparation for when we go back to using combinators instead of pattern
substitution.  Note that when we do use combinators again, they'll be of a very
eager or "aggressive" variety, so they should be as fast as substitution, or
even faster because I think they'll avoid extra work in cases where a value is
not used during computation. */

value remove_sym(value x, value f)
	{
	if (sym_eq(x,f))
		return I;
	else if (f->T != type_open)
		return f;
	else if (f->L->T == type_name || f->L->T == type_string)
		return f;
	else
		return apply(remove_sym(x,f->L), remove_sym(x,f->R));
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
			val = apply(C,f);
		else if (p->T == reduce_I)
			val = I;
		else if (p->L->T == reduce_C && p->R->T == reduce_I)
			/* Special case for the rule:  lam (C I) (L R) = L.  This greatly
			compresses many parsed forms, saving memory and time.  We copy f->L
			because it's a part of f and we'll be dropping f.  */
			val = copy(f->L);
		else
			val = apply(A(lam,p),remove_sym(x,f));

		drop(p);
		}

	drop(f);
	return val;
	}

static value check_apply(value f, value x)
	{
	if (f == I)
		return x;
	else if (f == 0)
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
	else
		return apply(f,x);
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

	value sym = new_parse_sym(allow_eq);
	hold(sym);

	value val;

	if (sym == 0)
		val = 0;
	else if (is_null_name(sym))
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
				def = check_apply(Y,lambda(sym, parse_term()));
				}
			else
				{
				/* Non-recursive definition */
				skip_filler();
				def = parse_term();
				}

			val = def ? lambda(sym, parse_exp()) : 0;
			val = check_apply(val,def);
			}
		else
			/* Normal parameter (symbol without definition) */
			val = lambda(sym, parse_exp());
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

			exp = check_apply(exp,val);

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

/* I'm temporarily making this return the symbols in reverse order, since we're
currently stacking them in reverse order in the final phase.  The next version
will switch back to forward order and eliminate the reversal in the final
phase.  I'm just dealing with one thing at a time here. */
value next_sym(value f)
	{
	if (f == 0)
		return 0;
	else if (f->T == type_open)
		{
		if (f->L->T == type_name || f->L->T == type_string)
			return f;
		else
			{
			value x = next_sym(f->R);
			if (x)
				return x;
			else
				return next_sym(f->L);
			}
		}
	else
		return 0;
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

	/* LATER I'll soon eliminate the need for symbols here. */
	value symbols = C;

	while (1)
		{
		value sym = next_sym(exp);
		if (sym == 0)
			break;

		symbols = item(sym->R,symbols);
		exp = lambda(sym,exp);
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
