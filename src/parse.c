#include <str.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <ctype.h>
#include <die.h>
#include <input.h>
#include <limits.h>
#include <parse.h>
#include <source.h>
#include <type_sym.h>

/*
Grammar:
[
exp    => empty
exp    => \ sym def exp
exp    => ; exp
exp    => term exp

term   => ( exp )
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

static int ch; /* current character */

static void skip(void)
	{
	ch = getd();
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
static value parse_name()
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
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| ch == '='
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

static value parse_symbol()
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name();
	}

static value parse_exp(void);

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
	else
		exp = parse_symbol();

	return exp;
	}

/* Parse a lambda form following the initial '\' character. */
static value parse_lambda(unsigned long first_line)
	{
	value sym;
	value def = 0;
	int lazy = 0;
	value body;
	value result;

	skip_white();

	/* Parse the symbol (function parameter). */
	sym = parse_symbol();
	if (sym == 0)
		syntax_error("Missing symbol after '\\'", first_line);

	skip_filler();
	first_line = source_line;

	/* Parse the definition of the symbol if we see an '=' char. */
	if (ch == '=')
		{
		skip();
		if (ch == '=')
			{
			lazy = 1;
			skip();
			}
		skip_filler();

		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	body = lam(sym,parse_exp());

	/* Produce the result based on the kind of definition used, if any. */
	if (def == 0)
		result = hold(body);
	else if (lazy)
		result = app(hold(body),hold(def));
	else
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
		{
		value term = parse_term();
		if (ch == '=')
			syntax_error("Missing symbol declaration before '='", source_line);
		return term;
		}
	}

static value parse_exp(void)
	{
	value exp = 0;
	while (1)
		{
		value factor = parse_factor();
		if (factor == 0) break;
		exp = (exp == 0) ? factor : app(exp,factor);
		}
	if (exp == 0) return hold(I);
	return exp;
	}

value parse(void)
	{
	value exp;

	/* Save and restore so getd can potentially call parse via eval. */
	int save_ch = ch;
	ch = 0;

	exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", source_line);

	ch = save_ch;
	return exp;
	}
