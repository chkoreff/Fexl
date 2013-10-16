#include <ctype.h>
#include <stdio.h>
#include "buf.h"
#include "die.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "form.h"
#include "long.h"
#include "parse.h"
#include "qfile.h"
#include "qstr.h"

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

If you're parsing a possibly malicious script, you should use setrlimit to
impose limits not only on stack depth, but also on total memory usage and CPU
time.

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

FILE *source_fh = 0;           /* current source file */
const char *source_name = 0;   /* current name of source file */
long source_line;              /* current line number */
static int ch;                 /* current character */

static void next_ch(void)
	{
	ch = fgetc(source_fh);
	if (ch == '\n') source_line++;
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

static void syntax_error(const char *msg, int line)
	{
	die("%s on line %ld%s%s", msg, line,
		source_name[0] ? " of " : "",
		source_name);
	}

/* Collect the content of a string up to the given terminator. */
static value collect_string(const char *term_string, long term_len,
	long first_line)
	{
	struct buf buffer;
	buf_start(&buffer);
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
				return symbol(C,buf_finish(&buffer),first_line);
			else
				{
				long i;
				for (i = 0; i < match_pos; i++)
					buf_add(&buffer, term_string[i]);

				match_pos = 0;
				}
			}
		else if (ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			{
			buf_add(&buffer, ch);
			next_ch();
			}
		}
	}

static value parse_simple_string(void)
	{
	long first_line = source_line;
	next_ch();
	return collect_string("\"", 1, first_line);
	}

static value parse_complex_string(void)
	{
	long first_line = source_line;
	struct buf buffer;
	buf_start(&buffer);

	while (1)
		{
		if (at_white())
			{
			struct str *term = buf_finish(&buffer);
			next_ch();
			value sym = collect_string(term->data, term->len, first_line);
			str_free(term);
			return sym;
			}
		else if (ch == -1)
			syntax_error("Unclosed string terminator", first_line);
		else
			{
			buf_add(&buffer, ch);
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
	long first_line = source_line;
	struct buf buffer;
	buf_start(&buffer);

	while (1)
		{
		if (
			at_white()
			|| ch == '\\'
			|| ch == '(' || ch == ')'
			|| ch == '[' || ch == ']'
			|| ch == '{' || ch == '}'
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| (ch == '=' && !allow_eq)
			|| ch == -1
			)
			return symbol(F,buf_finish(&buffer),first_line);
		else
			{
			buf_add(&buffer, ch);
			next_ch();
			}
		}
	}

/* Parse a symbol. */
static value parse_symbol(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

static value parse_term(void);
static value parse_exp(void);

static value parse_list(void)
	{
	skip_filler();
	if (ch == ';')
		{
		next_ch();
		return parse_exp();
		}

	value x = parse_term();
	hold(x);

	value g;
	if (is_null_name(x))
		g = C;
	else
		g = apply(apply(Qitem,x),parse_list());

	drop(x);
	return g;
	}

static value parse_term(void)
	{
	if (ch == '(') /* parenthesized expression */
		{
		long first_line = source_line;
		next_ch();
		value exp = parse_exp();
		if (ch != ')')
			syntax_error("Unclosed parenthesis", first_line);

		next_ch();
		return exp;
		}
	else if (ch == '[') /* list */
		{
		long first_line = source_line;
		next_ch();
		value exp = parse_list();
		if (ch != ']')
			syntax_error("Unclosed bracket", first_line);

		next_ch();
		return exp;
		}
	else if (ch == '{') /* quoted form */
		{
		long first_line = source_line;
		next_ch();
		value exp = parse_exp();
		if (ch != '}')
			syntax_error("Unclosed brace", first_line);

		next_ch();
		return A(I,exp);
		}
	else
		return parse_symbol(1);
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
static value parse_lambda(long first_line)
	{
	int allow_eq = at_white();
	skip_white();

	/* Parse the symbol (function parameter). */
	value sym = parse_symbol(allow_eq);
	if (is_null_name(sym))
		syntax_error("Missing lambda symbol", first_line);

	hold(sym);
	skip_filler();

	first_line = source_line;

	/* Count any '=' signs, up to 2. */
	int count_eq = 0;
	while (ch == '=' && count_eq < 2)
		{
		count_eq++;
		next_ch();
		}

	/* Parse the definition of the symbol if we saw an '=' char. */
	value def = 0;
	if (count_eq)
		{
		skip_filler();
		def = parse_term();

		if (is_null_name(def))
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	value body = abstract(sym,parse_exp());

	/* Produce the result based on the kind of definition used, if any. */
	value result;
	if (count_eq == 0)
		/* no definition */
		result = V(body->T,body->L,body->R); /* Make a copy. */
	else if (count_eq == 1)
		/* = normal definition */
		result = apply(body,def);
	else
		/* == eager definition */
		result = apply(apply(query,def),body);

	drop(body);
	drop(sym);
	return result;
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static value parse_factor(void)
	{
	skip_filler();

	if (ch == -1 || ch == ')' || ch == ']' || ch == '}')
		return 0;
	else if (ch == '\\')
		{
		long first_line = source_line;
		next_ch();
		if (ch == '\\')
			{
			ch = -1; /* Two backslashes simulates end of file. */
			return 0;
			}
		else
			return parse_lambda(first_line);
		}
	else if (ch == ';')
		{
		next_ch();
		return parse_exp();
		}
	else
		return parse_term();
	}

/* Parse an expression. */
static value parse_exp(void)
	{
	value exp = I;

	while (1)
		{
		value val = parse_factor();
		if (val == 0) return exp;
		exp = exp == I ? val : apply(exp,val);
		}
	}

value parse(void)
	{
	ch = 0;
	value exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", source_line);
	return exp;
	}

/* (parse fh name line)
Parse the stream fh with the given source name and initial line number.
Return (pair exp line), where exp is the parsed expression, and line is the
updated line number. */
value fexl_parse(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;

	value x = eval(f->L->L->R);
	value y = eval(f->L->R);
	value z = eval(f->R);

	FILE *save_fh = source_fh;
	const char *save_name = source_name;
	long save_line = source_line;

	source_fh = get_file(x);
	source_name = get_str(y)->data;
	source_line = get_long(z);

	value exp = parse();
	value result = pair(exp,Qlong(source_line));

	source_fh = save_fh;
	source_name = save_name;
	source_line = save_line;

	drop(x);
	drop(y);
	drop(z);

	return result;
	}
