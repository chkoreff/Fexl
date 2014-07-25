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
static unsigned long first_line; /* first line of a construct */
static const char *error_code;

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

/*
Parse a name, returning a symbol if we saw one, F if we didn't, or 0 if we ran
out of memory.

A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest rule that can work.
*/
static value parse_name(void)
	{
	buffer buf = 0;
	first_line = source_line;

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

static string collect_string(const char *end_data, unsigned long end_len)
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
			error_code = "Unclosed string";
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
	first_line = source_line;
	skip();
	return Qsym(1, collect_string("\"",1), first_line);
	}

static value parse_tilde_string(void)
	{
	buffer buf = 0;
	first_line = source_line;

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
		error_code = "Incomplete string terminator";
		buf_free(buf);
		return 0;
		}

	{
	string end = buf_finish(buf);
	if (!end) return 0;

	skip();
	{
	string content = collect_string(end->data, end->len);
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

static value parse_term()
	{
	return parse_symbol();
	}

/* Parse the next factor of an expression.  Return F if no factor found, or
0 if out of memory. */
static value parse_factor(void)
	{
	skip_filler();
	if (ch == -1)
		return F;
	else
		return parse_term();
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
			if (exp != I)
				{
				hold(exp);
				drop(exp);
				}
			exp = 0;
			break;
			}
		if (factor == F) break;
		exp = (exp == I) ? factor : app(exp,factor);
		}
	remain_depth++;
	return exp;
	}

/* Parse an expression and return (\: : exp error_code first_line). */
static value parse_form(void)
	{
	value exp = parse_exp();
	exp = A(A(L,I),exp ? exp : F);
	exp = A(A(L,exp),Qstr(str_new_data0(error_code ? error_code : "")));
	exp = A(A(L,exp),Qnum_ulong(first_line));
	return exp;
	}

value parse(input get)
	{
	const input save_getd = getd;
	const int save_ch = ch;
	const unsigned long save_source_line = source_line;
	const unsigned long save_first_line = first_line;
	const char *const save_error_code = error_code;
	value exp;

	getd = get;
	ch = 0;
	source_line = 1;

	exp = parse_form();

	getd = save_getd;
	ch = save_ch;
	source_line = save_source_line;
	first_line = save_first_line;
	error_code = save_error_code;

	return exp;
	}
