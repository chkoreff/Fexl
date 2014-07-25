#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <ctype.h>
#include <input.h>
#include <num.h>
#include <parse.h>
#include <type_num.h>
#include <type_str.h>
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
static const char *error_code;
static unsigned long error_line;

static void skip(void)
	{
	if (remain_steps > 0) remain_steps--; else { ch = -1; return; }
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

static void syntax_error(const char *code, unsigned long line)
	{
	error_code = code;
	error_line = line;
	}

/*
Parse a name, returning a symbol if we saw one, F if we didn't, or 0 if we ran
out of memory.

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
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| ch == '='
			|| ch == -1)
			break;

		buf = buf_add(buf,ch);
		if (!buf) return 0;
		skip();
		}

	if (!buf) return F;
	return Qsym(0, buf_finish(buf), first_line);
	}

static string collect_string(
	const char *end_data,
	unsigned long end_len,
	unsigned long first_line
	)
	{
	unsigned long match_pos = 0;
	buffer buf = 0;

	while (1)
		{
		if (match_pos >= end_len)
			break;

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
				{
				buf = buf_add(buf, end_data[i]);
				if (!buf) return 0;
				}

			match_pos = 0;
			}
		else if (ch == -1)
			{
			syntax_error("Unclosed string", first_line);
			buf_free(buf);
			return 0;
			}
		else
			{
			buf = buf_add(buf,(char)ch);
			if (!buf) return 0;
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
		if (!buf) return 0;
		}

	if (ch == -1)
		{
		syntax_error("Incomplete string terminator", first_line);
		buf_free(buf);
		return 0;
		}

	skip();

	{
	string end = buf_finish(buf);
	if (!end) return 0;
	{
	string content = collect_string(end->data, end->len, first_line);
	str_free(end);
	return Qsym(1,content,first_line);
	}
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

static value parse_exp(void);

static value parse_term()
	{
	unsigned long first_line = source_line;
	value exp;

	if (ch == '(') /* parenthesized expression */
		{
		skip();
		exp = parse_exp();
		if (!exp) return 0;
		if (ch != ')')
			{
			syntax_error("Unclosed parenthesis", first_line);
			return A(0,exp);
			}
		skip();
		}
	else
		exp = parse_symbol();

	return exp;
	}

/* Parse a lambda form following the initial '\' character. */
static value parse_lambda(unsigned long first_line)
	{
	skip_white();
	{
	/* Parse the symbol (function parameter). */
	value sym = parse_symbol();
	if (sym == F)
		{
		syntax_error("Missing symbol after '\\'", first_line);
		return 0;
		}

	skip_filler();
	first_line = source_line;

	{
	/* Parse the definition of the symbol if we see an '=' char. */
	value def = F;
	if (ch == '=')
		{
		skip();
		skip_filler();

		def = parse_term();
		if (def == F)
			{
			syntax_error("Missing definition", first_line);
			sym = A(0,sym);
			return 0;
			}
		}

	{
	/* Parse the body of the function. */
	value body = lam(sym,parse_exp());
	/* Return the body applied to the definition, if any. */
	return def == F ? body : app(body,def);
	}
	}
	}
	}

/* Parse the next factor of an expression.  Return F if no factor found, or 0
on error. */
static value parse_factor(void)
	{
	skip_filler();
	if (ch == -1)
		return F;
	else if (ch == '\\')
		{
		unsigned long first_line = source_line;
		skip();
		if (ch == '\\')
			{
			ch = -1; /* Two backslashes simulates end of file. */
			return F;
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
			{
			syntax_error("Missing symbol declaration before '='", source_line);
			term = A(0,term);
			}
		return term;
		}
	}

static value parse_exp(void)
	{
	value exp = I;
	if (remain_depth > 0) remain_depth--; else return 0;
	while (1)
		{
		value factor = parse_factor();
		if (!factor)
			{
			exp = A(0,exp);
			break;
			}
		if (factor == F) break;
		exp = (exp == I) ? factor : app(exp,factor);
		}
	remain_depth++;
	return exp;
	}

/* Parse an expression and return (\: : exp error_code error_line). */
static value parse_form(void)
	{
	value exp;
	if (getd)
		exp = parse_exp();
	else
		{
		syntax_error("Missing file", source_line);
		exp = 0;
		}

	if (ch != -1 && exp)
		{
		syntax_error("Extraneous input", source_line);
		exp = A(0,exp);
		}

	exp = A(A(L,I),exp ? exp : F);
	exp = A(A(L,exp),Qstr(str_new_data0(error_code ? error_code : "")));
	exp = A(A(L,exp),Qnum_ulong(error_line));
	return exp;
	}

value parse(input get)
	{
	const input save_getd = getd;
	const int save_ch = ch;
	const unsigned long save_source_line = source_line;
	const char *const save_error_code = error_code;
	const unsigned long save_error_line = error_line;
	value exp;

	getd = get;
	ch = 0;
	source_line = 1;

	exp = parse_form();

	getd = save_getd;
	ch = save_ch;
	source_line = save_source_line;
	error_code = save_error_code;
	error_line = save_error_line;

	return exp;
	}
