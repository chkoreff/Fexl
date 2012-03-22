#include <ctype.h>
#include <stdio.h>
#include "value.h"
#include "basic.h"
#include "buf.h"
#include "die.h"
#include "memory.h"
#include "parse.h"
#include "resolve.h"
#include "run.h"
#include "stack.h"
#include "string.h"
#include "sym.h"

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

int ch = 0;      /* current character */

/* The read_ch function reads the next character into the ch variable.  It
can be set to read from any source such as a file or a string. */
void (*read_ch)(void);

void next_ch(void)
	{
	read_ch();
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

struct value *collect_string(char *term_string, long term_len, long first_line)
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

struct value *parse_simple_string(void)
	{
	long first_line = line;
	next_ch();
	return collect_string("\"", 1, first_line);
	}

struct value *parse_complex_string(void)
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

	struct value *str = collect_string(term_string, term_len, first_line);
	free_memory(term_string, term_len+1);
	return str;
	}

/*
A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest possible rule that
can work.
*/
struct value *parse_name(int allow_eq)
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
struct value *parse_sym(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

/* stack of symbols defined outside the source text */
struct value *outside = 0;

struct value *find_sym(struct value *name)
	{
	struct value *pos = stack;
	while (1)
		{
		if (pos == 0) return 0;
		if (sym_eq(name,pos->L)) return pos->L;
		pos = pos->R;
		}
	}

extern struct value *parse_exp(void);

struct value *parse_term(void)
	{
	long first_line = line;
	if (ch == '(')
		{
		next_ch();
		struct value *exp = parse_exp();

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
		struct value *sym = parse_sym(1);

		if (sym->T == type_name && string_len(sym) == 0)
			{
			line = first_line;
			die("Missing definition");
			}

		struct value *def = find_sym(sym);

		if (!def)
			{
			/*TODO clean up (make a routine) */
			struct value *save = stack;
			stack = outside;

			def = find_sym(sym);
			if (!def)
				{
				def = sym;
				hold(def);
				push(def);
				}

			outside = stack;
			stack = save;
			}

		if (def != sym)
			{
			hold(sym);
			drop(sym);
			}

		return def;
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
struct value *parse_lambda(void)
	{
	int allow_eq = at_white();
	skip_white();

	struct value *sym = parse_sym(allow_eq);
	if (sym->T == type_name && string_len(sym) == 0)
		die("Missing lambda symbol");

	hold(sym);

	skip_filler();
	int has_def = (ch == '=');

	struct value *val;

	if (has_def)
		{
		next_ch();
		if (ch == '=')
			{
			/* Recursive definition */
			next_ch();
			skip_filler();

			push(sym);
			struct value *def = parse_term();
			def = A(Y,lambda(sym,def));
			val = lambda(sym,parse_exp());
			val = A(val,def);
			pop();
			}
		else
			{
			/* Non-recursive definition, forces eager evaluation */
			skip_filler();
			struct value *def = parse_term();

			push(sym);
			val = lambda(sym,parse_exp());
			val = A(A(query,def),val);
			pop();
			}
		}
	else
		{
		/* Normal parameter (symbol without definition) */
		push(sym);
		val = lambda(sym,parse_exp());
		pop();
		}

	drop(sym);
	return val;
	}

/*
Parse an expression.

We limit the depth here to avoid a stack overflow resulting from strange inputs
like an infinite series of left parentheses.
*/
struct value *parse_exp(void)
	{
	struct value *exp = I;

	if (++cur_depth > max_depth)
		die("Your program is too deeply nested.");

	while (1)
		{
		skip_filler();
		if (ch == EOF || ch == ')') break;

		struct value *val;

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

		exp = exp->T == type_I ? val : A(exp,val);
		}

	cur_depth--;
	return exp;
	}

struct value *parse_source(void)
	{
	line = 1;
	next_ch();

	struct value *save_stack = stack;
	stack = 0;

	struct value *exp = parse_exp();

	if (ch != EOF)
		die("Extraneous input");

	int ok = 1;
	stack = outside;
	while (stack)
		{
		struct value *x = stack->L;
		exp = Qresolve(x,exp); /*TODO*/
		drop(x);
		pop();
		}

	outside = 0;

	if (!ok)
		die("Your program has undefined symbols."); /*TODO*/

	stack = save_stack;

	return exp;
	}
