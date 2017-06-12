#include <str.h>

#include <input.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <ctype.h> /* isspace iscntrl */
#include <parse.h>
#include <report.h>
#include <type_form.h>
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
exp    => \ ; exp

term   => ( exp )
term   => [ list ]
term   => { tuple }
term   => sym

list   => empty
list   => term list
list   => ; exp

tuple  => empty
tuple  => term tuple

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
static unsigned long line; /* current line number */
static input get; /* current input routine */
static void *source; /* current input source */
static value label; /* label of current input source */

static void skip(void)
	{
	ch = get(source);
	if (ch == '\n')
		line++;
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
	return isspace(ch) || iscntrl(ch);
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

static void syntax_error(const char *code, unsigned long line)
	{
	fatal_error(code,line,str_data(label));
	}

/* Parse a name, or return 0 if I don't see one.

A name may contain just about anything, except for white space and a few other
special characters.  This is the simplest rule that can work.
*/
static value parse_name(void)
	{
	struct buffer buf = {0};
	unsigned long first_line = line;

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

		buf_add(&buf,(char)ch);
		skip();
		}

	if (!buf.top) return 0;
	return Qsym(buf_clear(&buf), first_line);
	}

/* Collect a string up to an ending terminator. */
static string collect_string(
	const char *end_data,
	unsigned long end_len,
	unsigned long first_line
	)
	{
	unsigned long match_pos = 0;
	struct buffer buf = {0};

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
			buf_addn(&buf,end_data,match_pos);
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

	return buf_clear(&buf);
	}

static value parse_quote_string(void)
	{
	unsigned long first_line = line;
	skip();
	return Qstr(collect_string("\"",1,first_line));
	}

/* Parse a tilde string terminator. */
static string parse_terminator(unsigned long first_line)
	{
	struct buffer buf = {0};
	while (1)
		{
		if (ch == -1 || at_white())
			break;
		buf_add(&buf,(char)ch);
		skip();
		}

	if (ch == -1)
		syntax_error("Incomplete string terminator", first_line);

	skip();
	return buf_clear(&buf);
	}

/* Gather string content up to the next occurrence of terminator. */
static string parse_content(string end, unsigned long first_line)
	{
	string content = collect_string(end->data, end->len, first_line);
	str_free(end);
	return content;
	}

static value parse_tilde_string(void)
	{
	unsigned long first_line = line;
	string end = parse_terminator(first_line);
	string content = parse_content(end,first_line);
	return Qstr(content);
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
	skip_filler();
	if (ch == ';')
		{
		skip();
		return parse_exp();
		}
	else
		{
		value term = parse_term();
		if (term == 0)
			return hold(Qnull);
		else
			return app(app(hold(Qcons),term),parse_list());
		}
	}

static value parse_tuple(void)
	{
	value pattern = hold(QT);
	value exp = hold(QI);
	while (1)
		{
		value term;
		skip_filler();
		term = parse_term();
		if (term == 0) break;
		pattern = A(pattern,hold(QF));
		exp = app(exp,term);
		}
	return Qsubst(pattern,exp);
	}

static value parse_term(void)
	{
	value exp;
	unsigned long first_line = line;
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
	else if (ch == '{') /* tuple */
		{
		skip();
		exp = parse_tuple();
		if (ch != '}')
			syntax_error("Unclosed brace", first_line);
		skip();
		}
	else
		exp = parse_symbol();

	return exp;
	}

/* Parse a lambda form following the initial '\' character. */
static value parse_lambda(unsigned long first_line)
	{
	value sym, def=0, exp;

	/* Parse the symbol. */
	skip_white();
	sym = parse_name();
	if (sym == 0)
		syntax_error("Missing symbol after '\\'", first_line);

	/* Parse the optional definition of the symbol. */
	skip_filler();
	first_line = line;
	if (ch == '=')
		{
		skip();
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	exp = parse_exp();
	exp = lambda(sym_name(sym),exp);
	drop(sym);

	/* Apply the definition if any. */
	if (def == 0)
		return exp;
	else
		return app(app(hold(Qeval),def),exp);
	}

/* Parse unresolved form. */
static value parse_form(void)
	{
	value exp = parse_exp();
	return Qform(hold(label),exp);
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static value parse_factor(void)
	{
	skip_filler();
	if (ch == -1)
		return 0;
	else if (ch == '\\')
		{
		unsigned long first_line = line;
		skip();
		if (ch == '\\')
			{
			ch = -1; /* Two backslashes simulates end of file. */
			return 0;
			}
		else if (ch == ';')
			{
			skip();
			return parse_form();
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
		value factor = parse_term();
		if (ch == '=')
			syntax_error("Missing symbol declaration before '='", line);
		return factor;
		}
	}

/* Parse an expression. */
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
	if (exp == 0) exp = hold(QI);
	return exp;
	}

static value parse_input(void)
	{
	value exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", line);
	return exp;
	}

/* Parse the given input. */
value parse(input _get, void *_source, value _label)
	{
	value exp;

	get = _get;
	source = _source;
	label = _label;
	ch = ' ';
	line = 1;

	exp = Qform(_label,parse_input());

	return exp;
	}
