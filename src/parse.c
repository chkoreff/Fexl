#include <str.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <ctype.h>
#include <die.h>
#include <limits.h>
#include <parse.h>
#include <system.h>
#include <type_sym.h>

/*
Grammar:
[
exp    => terms
exp    => \ sym def exp

terms  => empty
terms  => term terms
terms  => ; exp

term   => ( exp )
term   => [ terms ]
term   => { exp }
term   => sym

def    => empty
def    => = term
def    => == term

sym    => name
sym    => string

string => simple_string
string => complex_string
]

The Fexl parser reads an expression from the input until it reaches -1 (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.
*/

/* The read_ch function reads the next character.  It can be set to read from
any source such as a file or a string. */
int (*read_ch)(void);

static int ch; /* current character */

static void skip(void)
	{
	ch = read_ch();
	if (ch == '\n')
		{
		if (source_line == ULONG_MAX) die("skip");
		source_line++;
		}
	}

static void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == -1)
			{
			skip();
			return;
			}
		else
			skip();
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
			skip();
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

/*
A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest possible rule that
can work.
*/
static value parse_name(int allow_eq)
	{
	unsigned long first_line = source_line;
	buffer buf;
	string name;

	buf_start(&buf);
	while (1)
		{
		if (at_white()
			|| ch == '\\'
			|| ch == '(' || ch == ')'
			|| ch == '[' || ch == ']'
			|| ch == '{' || ch == '}'
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| (ch == '=' && !allow_eq)
			|| ch == -1)
			break;
		buf_add(&buf,(char)ch);
		skip();
		}

	name = buf_finish(&buf);
	return name ? Qsym(0,name,first_line) : 0;
	}

static string collect_string(
	const char *term_data,
	unsigned long term_len,
	unsigned long first_line)
	{
	unsigned long match_pos = 0;
	buffer buf;
	string str;

	buf_start(&buf);
	while (1)
		{
		if (match_pos >= term_len)
			break;

		if (ch == term_data[match_pos])
			{
			match_pos++;
			skip();
			}
		else if (match_pos > 0)
			{
			/* Buffer the ones matched so far and start over. */
			unsigned long i;
			for (i = 0; i < match_pos; i++)
				buf_add(&buf, term_data[i]);

			match_pos = 0;
			}
		else if (ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			{
			buf_add(&buf,(char)ch);
			skip();
			}
		}

	str = buf_finish(&buf);
	if (str == 0) str = str_new_data("",0);
	return str;
	}

static value parse_simple_string(void)
	{
	unsigned long first_line = source_line;
	string str;
	skip();
	str = collect_string("\"", 1, first_line);
	return Qsym(1,str,first_line);
	}

static value parse_complex_string(void)
	{
	unsigned long first_line = source_line;
	buffer buf;
	string term;
	string str;

	buf_start(&buf);
	while (1)
		{
		skip();
		if (ch == -1 || isspace(ch))
			break;
		buf_add(&buf,(char)ch);
		}

	term = buf_finish(&buf);
	if (ch == -1 || term == 0)
		syntax_error("Incomplete string terminator", first_line);

	skip();
	str = collect_string(term->data, term->len, first_line);
	str_free(term);
	return Qsym(1,str,first_line);
	}

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
	value term;

	skip_filler();
	if (ch == ';')
		{
		skip();
		return parse_exp();
		}

	term = parse_term();
	if (term == 0) return hold(C);
	return app(app(hold(Qcons),term),parse_list());
	}

static value parse_term(void)
	{
	unsigned long first_line = source_line;
	value exp;

	if (ch == '(') /* parenthesized expression */
		{
		skip();
		exp = parse_exp();
		if (ch != ')')
			syntax_error("Unclosed parenthesis", first_line);
		skip();
		}
	else if (ch == '[') /* list */
		{
		skip();
		exp = parse_list();
		if (ch != ']')
			syntax_error("Unclosed bracket", first_line);
		skip();
		}
	else if (ch == '{') /* symbolic form */
		{
		skip();
		exp = A(hold(I),parse_exp());
		if (ch != '}')
			syntax_error("Unclosed brace", first_line);
		skip();
		}
	else
		exp = parse_symbol(1);

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
static value parse_lambda(unsigned long first_line)
	{
	value sym;
	unsigned short count_eq = 0;
	value def = 0;
	value body;
	value result;

	int allow_eq = at_white();
	skip_white();

	/* Parse the symbol (function parameter). */
	sym = parse_symbol(allow_eq);
	if (sym == 0)
		syntax_error("Missing lambda symbol", first_line);

	skip_filler();

	first_line = source_line;

	/* Count any '=' signs, up to 2. */
	while (ch == '=' && count_eq < 2)
		{
		count_eq++;
		skip();
		}

	/* Parse the definition of the symbol if we saw an '=' char. */
	if (count_eq)
		{
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	body = lam(sym,parse_exp());

	/* Produce the result based on the kind of definition used, if any. */
	if (count_eq == 0)
		/* no definition */
		result = hold(body);
	else if (count_eq == 1)
		{
		/* = normal definition */
		if (body == I)
			result = hold(def);
		else
			result = app(hold(body),hold(def));
		}
	else
		/* == eager definition */
		result = app(app(hold(query),hold(def)),hold(body));

	if (def) drop(def);
	drop(body);
	return result;
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static value parse_factor(void)
	{
	skip_filler();
	if (ch == -1)
		return 0;
	else if (ch == '\\')
		{
		unsigned long first_line = source_line;
		skip();
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
		skip();
		return parse_exp();
		}
	else
		return parse_term();
	}

static value parse_exp(void)
	{
	value exp = 0;
	while (1)
		{
		value val = parse_factor();
		if (val == 0) break;
		exp = (exp == 0) ? val : app(exp,val);
		}
	if (exp == 0) return hold(I);
	return exp;
	}

value parse(void)
	{
	value exp;

	/* Save and restore so read_ch can potentially call parse via eval. */
	int save_ch = ch;
	ch = 0;

	exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", source_line);

	ch = save_ch;
	return exp;
	}
