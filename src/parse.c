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

const char *error_code;
unsigned long error_line;

static void skip(void)
	{
	if (remain_steps) remain_steps--; else { ch = -1; return; }
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

		buf = buf_add(buf,(char)ch);
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
	if (ch == '(') /* parenthesized expression */
		{
		value exp;
		skip();
		exp = parse_exp();
		if (!exp) return 0;
		if (ch != ')')
			{
			drop(exp);
			syntax_error("Unclosed parenthesis", first_line);
			return 0;
			}
		skip();
		return exp;
		}
	else
		return parse_symbol();
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
	unsigned short count_eq = 0;
	value def = F;
	if (ch == '=')
		{
		count_eq++;
		skip();
		if (ch == '=')
			{
			count_eq++;
			skip();
			}
		skip_filler();
		def = parse_term();
		if (def == F)
			{
			syntax_error("Missing definition", first_line);
			drop(sym);
			return 0;
			}
		}

	{
	/* Parse the body of the function and apply the definition if any. */
	value body = lam(sym,parse_exp());
	if (count_eq == 0)
		return body; /* no def */
	else if (count_eq == 1)
		return app(app(hold(Qeval),def),body); /* eager */
	else
		return app(body,def); /* lazy */
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
		value factor = parse_term();
		if (ch == '=')
			{
			syntax_error("Missing symbol declaration before '='", source_line);
			if (factor != F) drop(factor);
			return 0;
			}
		return factor;
		}
	}

static value parse_exp(void)
	{
	value exp = hold(I);
	if (remain_depth) remain_depth--; else return 0;
	while (1)
		{
		value factor = parse_factor();
		if (factor == F) break;
		exp = app(exp,factor);
		if (!exp) break;
		}
	remain_depth++;
	return exp;
	}

/* Parse the source stream. */
value parse_source()
	{
	ch = 0;
	source_line = 1;

	if (getd)
		{
		value exp = parse_exp();
		if (ch != -1 && exp)
			{
			drop(exp);
			syntax_error("Extraneous input", source_line);
			exp = 0;
			}
		return exp;
		}
	else
		{
		syntax_error("Could not open the input file", source_line);
		return 0;
		}
	}

/*
(embed_parse ok err) =
	(ok form)       # if ok
	(err msg line)  # if syntax error
*/
value embed_parse(value ok, value err)
	{
	value result;
	const int save_ch = ch;
	const unsigned long save_source_line = source_line;
	{
	value exp = parse_source();
	if (exp)
		{
		result = A(ok,exp);
		drop(err);
		}
	else
		{
		if (!error_code) error_code = "";
		result = A(A(err,Qstr(str_new_data0(error_code))),
			Qnum_ulong(error_line));
		drop(ok);
		}
	/* Clear error so main doesn't report it. */
	error_code = 0;
	error_line = 0;
	}
	ch = save_ch;
	source_line = save_source_line;
	return result;
	}
