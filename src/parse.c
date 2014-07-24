#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <ctype.h>
#include <input.h>
#include <parse.h>
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

string => simple_string
string => complex_string
]

The Fexl parser reads an expression from the input until it reaches -1 (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.
*/

static int ch; /* current character */
static unsigned long source_line;  /* current line number */

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
and a few other special characters.  This is the simplest possible rule that
can work.
*/
static value parse_name(void)
	{
	unsigned long first_line = source_line;
	buffer buf = 0;

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
	return Qsym(0,Qstr(buf_finish(buf)),first_line);
	}

static value parse_symbol(void)
	{
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

value parse(input get)
	{
	const input save_getd = getd;
	const int save_ch = ch;
	const unsigned long save_source_line = source_line;
	value f;

	getd = get;
	ch = 0;
	source_line = 1;

	f = parse_exp();

	getd = save_getd;
	ch = save_ch;
	source_line = save_source_line;

	return f;
	}
