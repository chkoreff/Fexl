#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <ctype.h>
#include <die.h>
#include <input.h>
#include <output.h>
#include <parse.h>
#include <source.h>
#include <type_resolve.h>
#include <type_str.h>
#include <type_sym.h>

/*
Grammar:
[
exp    => empty
exp    => term exp
exp    => ; exp
exp    => \ sym exp
exp    => \ sym = term exp
exp    => \ sym == term exp
exp    => { exp } exp

term   => ( exp )
term   => [ list ]
term   => sym

list   => empty
list   => term list
list   => ; exp

sym    => name
sym    => string

string => quote_string
string => tilde_string
]

The Fexl parser reads an expression from the input until it reaches -1 (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.
*/

static int ch; /* current character */
static unsigned long source_line;  /* current line number */

static void syntax_error(const char *code, unsigned long line)
	{
	put_to_error();
	put(code); put_error_location(line);
	die(0);
	}

static void skip(void)
	{
	ch = getd();
	if (ch == '\n')
		source_line++;
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

/* Parse a name, or return 0 if we don't see one.

A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest rule that can work.
*/
static value parse_name(void)
	{
	buffer buf = 0;
	unsigned long first_line = source_line;

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
			|| ch == '='
			|| ch == -1)
			break;

		buf = buf_add(buf,(char)ch);
		skip();
		}

	if (!buf) return 0;
	return Qsym(0, buf_finish(buf), first_line);
	}

/* Collect a string up to an ending terminator. */
static string collect_string(
	const char *end_data,
	unsigned long end_len,
	unsigned long first_line
	)
	{
	unsigned long match_pos = 0;
	buffer buf = 0;

	while (match_pos < end_len)
		{
		if (ch == end_data[match_pos])
			{
			match_pos++;
			skip();
			}
		else if (match_pos > 0)
			{
			/* Buffer the ones matched so far and start over. */
			unsigned long i;
			for (i = 0; i < match_pos; i++)
				buf = buf_add(buf, end_data[i]);

			match_pos = 0;
			}
		else if (ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			{
			buf = buf_add(buf,(char)ch);
			skip();
			}
		}

	return buf_finish(buf);
	}

static value parse_quote_string(void)
	{
	unsigned long first_line = source_line;
	skip();
	return Qsym(1, collect_string("\"",1,first_line), first_line);
	}

static value parse_tilde_string(void)
	{
	buffer buf = 0;
	unsigned long first_line = source_line;

	while (1)
		{
		skip();
		if (ch == -1 || isspace(ch))
			break;
		buf = buf_add(buf,(char)ch);
		}

	if (ch == -1 || buf == 0)
		syntax_error("Incomplete string terminator", first_line);

	skip();

	{
	string end = buf_finish(buf);
	string content = collect_string(end->data, end->len, first_line);
	str_free(end);
	return Qsym(1,content,first_line);
	}
	}

static value parse_symbol(void)
	{
	if (ch == '"')
		return parse_quote_string();
	else if (ch == '~')
		return parse_tilde_string();
	else
		return parse_name();
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
	if (ch == '(') /* parenthesized expression */
		{
		value exp;
		skip();
		exp = parse_exp();
		if (ch != ')')
			syntax_error("Unclosed parenthesis", first_line);
		skip();
		return exp;
		}
	else if (ch == '[') /* list */
		{
		value exp;
		skip();
		exp = parse_list();
		if (ch != ']')
			syntax_error("Unclosed bracket", first_line);
		skip();
		return exp;
		}
	else
		return parse_symbol();
	}

/* Parse a lambda form following the initial '\' character. */
static value parse_lambda(unsigned long first_line)
	{
	value sym, def=0, exp;
	char is_recursive = 0;

	/* Parse the symbol (function parameter). */
	skip_white();
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
			is_recursive = 1;
			skip();
			}
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}
	if (is_recursive)
		def = app(hold(Y),lam(hold(sym),def));

	/* Parse the body of the function and apply the definition if any. */
	exp = lam(sym,parse_exp());
	if (def)
		exp = app(app(hold(Qeval),def),exp);
	return exp;
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
	else if (ch == '{')
		{
		/* Resolve in a context. */
		unsigned long first_line = source_line;
		value context, form, label;

		skip();
		context = parse_exp();
		if (ch != '}')
			syntax_error("Unclosed brace", first_line);

		skip();
		form = parse_exp();
		label = Qstr(str_new_data0(source_label ? source_label : ""));
		return app(A(A(Q(type_resolve),label),form),context);
		}
	else
		{
		value factor = parse_term();
		if (ch == '=')
			syntax_error("Missing symbol declaration before '='", source_line);
		return factor;
		}
	}

static value parse_exp(void)
	{
	value exp = 0;
	while (1)
		{
		value factor = parse_factor();
		if (factor == 0) break;
		if (exp == 0)
			exp = factor;
		else
			exp = app(exp,factor);
		}
	if (exp == 0) exp = hold(I);
	return exp;
	}

/* Parse the source stream. */
value parse_source(void)
	{
	value exp;

	ch = 0;
	source_line = 1;

	exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", source_line);
	return exp;
	}
