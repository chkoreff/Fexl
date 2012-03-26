#include <ctype.h>
#include <stdio.h>
#include "value.h"
#include "basic.h"
#include "buf.h"
#include "die.h"
#include "long.h"
#include "memory.h"
#include "parse.h"
#include "resolve.h"
#include "run.h"
#include "stack.h"
#include "string.h"
#include "sym.h"

static value C;
static value I;
static value Y;
static value query;

extern value parse_exp(void);
extern value lambda(value, value);

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

The Fexl parser reads an expression from the input until it reaches EOF (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.
*/

/* The read_ch function reads the next character.  It can be set to read from
any source such as a file or a string. */
int (*read_ch)(void);

static int ch = 0;      /* current character */

void next_ch(void)
	{
	ch = read_ch();
	if (ch == '\n') line++;
	}

void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == EOF) break;
		next_ch();
		}
	next_ch();
	}

int at_white(void)
	{
	return isspace(ch) || ch == 0;
	}

void skip_white(void)
	{
	while (1)
		{
		if (!at_white() || ch == EOF) break;
		next_ch();
		}
	}

void skip_filler(void)
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

value collect_string(char *term_string, long term_len, long first_line)
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
		else if (ch == EOF)
			{
			line = first_line;
			die("Unclosed string");
			}
		else
			{
			buf_add(&buf, ch);
			next_ch();
			}
		}

	long len;
	char *string = buf_clear(&buf,&len);
	return Qchars(string,len);
	}

value parse_simple_string(void)
	{
	long first_line = line;
	next_ch();
	return collect_string("\"", 1, first_line);
	}

value parse_complex_string(void)
	{
	long first_line = line;
	struct buf *term = 0;

	while (1)
		{
		if (at_white() || ch == EOF) break;
		buf_add(&term, ch);
		next_ch();
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
value parse_name(int allow_eq)
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
			|| ch == EOF
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
value parse_sym(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

value find_sym(value list, value name)
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

/* Return the originally encountered occurrence of a symbol.  This unifies
multiple occurrences into a single one. */
value orig_sym(value sym, long first_line)
	{
	value orig = find_sym(inner_syms,sym);

	if (!orig)
		{
		orig = find_sym(outer_syms,sym);

		if (!orig)
			{
			orig = sym;
			hold(orig);
			push_list(&outer_syms,orig); /* new outer sym encountered */

			value place = Qlong(first_line);
			hold(place);
			push_list(&outer_places,place);
			}
		}

	if (orig != sym)
		{
		hold(sym);
		drop(sym);
		}

	return orig;
	}

value parse_term(void)
	{
	long first_line = line;
	if (ch == '(')
		{
		next_ch();
		value exp = parse_exp();

		if (ch == ')')
			next_ch();
		else
			{
			line = first_line;
			die("Unclosed parenthesis");
			}

		return exp;
		}
	else
		{
		value sym = parse_sym(1);

		if (sym->T == type_name && string_len(sym) == 0)
			{
			line = first_line;
			die("Missing definition");
			}

		return orig_sym(sym,first_line);
		}
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
value parse_lambda(void)
	{
	int allow_eq = at_white();
	skip_white();

	value sym = parse_sym(allow_eq);
	if (sym->T == type_name && string_len(sym) == 0)
		die("Missing lambda symbol");

	hold(sym);

	skip_filler();
	int has_def = (ch == '=');

	value val;

	if (has_def)
		{
		next_ch();
		if (ch == '=')
			{
			/* Recursive definition */
			next_ch();
			skip_filler();

			push_list(&inner_syms,sym);
			value def = parse_term();
			def = A(Y,lambda(sym,def));
			val = lambda(sym,parse_exp());
			val = A(val,def);
			pop_list(&inner_syms);
			}
		else
			{
			/* Non-recursive definition, forces eager evaluation */
			skip_filler();
			value def = parse_term();

			push_list(&inner_syms,sym);
			val = lambda(sym,parse_exp());
			val = A(A(query,def),val);
			pop_list(&inner_syms);
			}
		}
	else
		{
		/* Normal parameter (symbol without definition) */
		push_list(&inner_syms,sym);
		val = lambda(sym,parse_exp());
		pop_list(&inner_syms);
		}

	drop(sym);
	return val;
	}

/*
Parse an expression.

We limit the depth here to avoid a stack overflow resulting from strange inputs
like an infinite series of left parentheses.
*/
value parse_exp(void)
	{
	value exp = I;

	if (++cur_depth > max_depth)
		die("Your program is too deeply nested.");

	while (1)
		{
		skip_filler();
		if (ch == EOF || ch == ')') break;

		value val;

		if (ch == '\\')
			{
			next_ch();
			if (ch == '\\')
				{
				ch = EOF; /* Two backslashes simulates end of file. */
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

		exp = exp->T == fexl_I ? val : A(exp,val);
		}

	cur_depth--;
	return exp;
	}

value get_pattern(value sym, value fun)
	{
	if (fun == sym) return I;
	if (!fun->L) return C;

	value fl = get_pattern(sym,fun->L);
	value fr = get_pattern(sym,fun->R);
	if (fl->T == fexl_C && fr->T == fexl_C) return C;

	return A(fl,fr);
	}

/* Return a copy of fun with val substituted according to pattern p. */
value subst(value p, value fun, value val)
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

value lambda(value x, value f)
	{
	return A(A(lam,get_pattern(x,f)),f);
	}

/*TODO pass resolution function into parse instead of always using built-in */
/* TODO use the version that chains */
static value resolve;

value Qresolve(value sym, value place, value exp)
	{
	return A(A(A(resolve,sym),place),lambda(sym,exp));
	}

void beg_parse(void)
	{
	C = Q(fexl_C);
	I = Q(fexl_I);
	Y = Q(fexl_Y);
	query = Q(fexl_query);
	lam = Q(fexl_lambda);
	resolve = Q(fexl_resolve);

	hold(C);
	hold(I);
	hold(Y);
	hold(query);
	hold(lam);
	hold(resolve);
	}

void end_parse(void)
	{
	drop(C);
	drop(I);
	drop(Y);
	drop(query);
	drop(lam);
	drop(resolve);
	}

value parse_source(void)
	{
	beg_parse();

	line = 1;
	next_ch();

	value exp = parse_exp();

	if (ch != EOF)
		die("Extraneous input");

	while (outer_syms)
		{
		value sym = outer_syms->L;
		value place = outer_places->L;

		exp = Qresolve(sym,place,exp);

		drop(sym);
		pop_list(&outer_syms);

		drop(place);
		pop_list(&outer_places);
		}

	end_parse();
	return exp;
	}
