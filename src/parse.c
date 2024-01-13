#include <stddef.h>

#include <buf.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <buf_str.h>
#include <parse.h>
#include <report.h>
#include <stream.h>
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>

/*
Fexl grammar:
[
exp     =>  empty
exp     =>  exp factor

factor  =>  "\;" exp
factor  =>  "\=" exp
factor  =>  "\" name def exp      # eager substitution
factor  =>  "\\" name def exp     # direct substitution
factor  =>  ";" exp
factor  =>  term

def     =>  empty
def     =>  "=" term

term    =>  "(" exp ")"
term    =>  "[" items "]"
term    =>  "{" items "}"
term    =>  string
term    =>  name

items   =>  empty
items   =>  term items
items   =>  ";" exp

string  =>  quote_string
string  =>  tilde_string
]

The parser reads an expression from the input until it reaches end of file or
the special token "\#" which acts like end of file.  It skips white space and
comments which appear before syntactic elements.  White space is any character
with an ASCII value less than or equal to ' ' (SPACE).  A comment starts with
'#' and continues through end of line.

The "empty" element means end of file.

A "name" is any sequence of characters other than white space or these:
  \ ( ) [ ] { } ; " ~ # =

A "quote_string" starts with a '"' (QU), followed by any sequence of characters
other than QU, followed by a closing QU.

A "tilde_string" is a sequence of arbitrary characters enclosed within a pair of
delimiters of your choice.  It starts with a delimiter, which is a '~' (tilde),
then zero or more non white characters, then a single white space which ends the
delimiter and is ignored.  That initial delimiter is followed by a sequence of
characters which constitute the actual content of the string, terminated by a
repeat occurrence of the delimiter.
*/

static value cur_label; // label of current input stream

static void syntax_error(const char *code, unsigned long line)
	{
	fatal_error(code,line,str_data(cur_label));
	}

// Parse a name, or return 0 if I don't see one.
//
// A name may contain just about anything, except for white space and a few
// other special chars.  This is the simplest rule that can work.
static string parse_name(void)
	{
	struct buffer buf = {0};
	while
		(
		cur_ch > ' '
		&& cur_ch != '\\'
		&& cur_ch != '(' && cur_ch != ')'
		&& cur_ch != '[' && cur_ch != ']'
		&& cur_ch != '{' && cur_ch != '}'
		&& cur_ch != ';'
		&& cur_ch != '"'
		&& cur_ch != '~'
		&& cur_ch != '#'
		&& cur_ch != '='
		)
		buf_keep(&buf);

	if (!buf.len) return 0;
	return buf_clear(&buf);
	}

static void error_unclosed(unsigned long first_line)
	{
	syntax_error("Unclosed string", first_line);
	}

static value parse_quote_string(void)
	{
	unsigned long first_line = cur_line;
	struct buffer s_buf = {0};
	struct buffer *buf = &s_buf;

	skip();
	if (!collect_to_ch(buf,'"'))
		{
		buf_discard(&s_buf);
		error_unclosed(first_line);
		}

	return quo(Qstr(buf_clear(&s_buf)));
	}

static value parse_tilde_string(void)
	{
	unsigned long first_line = cur_line;
	struct buffer buf = {0};

	int code = collect_tilde_string(&buf);
	if (code != 1)
		{
		buf_discard(&buf);
		if (code == -1)
			syntax_error("Incomplete string terminator", first_line);
		else
			error_unclosed(first_line);
		}

	return quo(Qstr(buf_clear(&buf)));
	}

static value parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return 0;
	// See if it's a numeric constant.
	{
	value n = Qnum_str0(name->data);
	if (n)
		{
		str_free(name);
		return quo(n);
		}
	else
		return ref(name,first_line);
	}
	}

static value parse_term(void);
static value parse_exp(void);

static value parse_nested(void)
	{
	unsigned long first_line = cur_line;
	value exp;
	skip();
	exp = parse_exp();
	if (cur_ch != ')')
		syntax_error("Unclosed parenthesis", first_line);
	skip();
	return exp;
	}

static value parse_items(void)
	{
	skip_filler();
	if (cur_ch == ';')
		{
		skip();
		return parse_exp();
		}
	else
		{
		value term = parse_term();
		if (term == 0)
			return quo(hold(Qnull));
		else
			return join(type_list,term,parse_items());
		}
	}

static value parse_seq(const char t_ch, const char *msg)
	{
	unsigned long first_line = cur_line;
	skip();
	{
	value exp = parse_items();
	if (cur_ch != t_ch)
		syntax_error(msg, first_line);
	skip();
	return exp;
	}
	}

// If a list is exactly two items then return a pair of those items.
static value check_pair(const value list)
	{
	value p = list;
	value x1, x2;

	if (p->T == type_quo) p = p->R;
	if (p->T != type_list) return 0;

	x1 = p->L;
	p = p->R;

	if (p->T == type_quo) p = p->R;
	if (p->T != type_list) return 0;

	x2 = p->L;
	p = p->R;

	if (p->T == type_quo) p = p->R;
	if (p->T != type_null) return 0;

	if (list->T == type_quo)
		return quo(V(type_pair,hold(x1),hold(x2)));
	else if (list->R->T == type_quo)
		return V(type_pair,hold(x1),quo(hold(x2)));
	else
		return join(type_pair,hold(x1),hold(x2));
	}

static value parse_tuple(void)
	{
	value list = parse_seq('}',"Unclosed brace");
	value pair = check_pair(list);
	if (pair)
		{
		drop(list);
		return pair;
		}
	else
		return join(type_tuple,quo(hold(Qtuple)),list);
	}

static value parse_list(void)
	{
	return parse_seq(']',"Unclosed bracket");
	}

static value parse_term(void)
	{
	if (cur_ch == '(')
		return parse_nested();
	else if (cur_ch == '[')
		return parse_list();
	else if (cur_ch == '{')
		return parse_tuple();
	else if (cur_ch == '"')
		return parse_quote_string();
	else if (cur_ch == '~')
		return parse_tilde_string();
	else
		return parse_symbol();
	}

// Parse a lambda form following the initial '\' character.
static value parse_lambda(unsigned long first_line, type type)
	{
	string name = parse_name();
	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	// Lambda name cannot be a numeric constant.
	{
	value n = Qnum_str0(name->data);
	if (n)
		syntax_error("Lambda name cannot be a number", first_line);
	}

	skip_filler();
	if (cur_ch != '=')
		return lam(type,name,parse_exp()); // No definition

	first_line = cur_line;
	skip();
	skip_filler();

	// Parse the definition.
	{
	value def = parse_term();
	if (def == 0)
		syntax_error("Missing definition", first_line);
	return app(lam(type,name,parse_exp()),def);
	}
	}

// Parse a form (unresolved symbolic expression).
static value parse_form(void)
	{
	return Qform(hold(cur_label),parse_exp());
	}

// Parse the next factor of an expression, if any.
static value parse_factor(void)
	{
	skip_filler();
	if (cur_ch == -1)
		return 0;
	else if (cur_ch == '\\')
		{
		unsigned long first_line = cur_line;
		skip();
		if (cur_ch == '#')
			{
			cur_ch = -1; // "\#" simulates end of file.
			return 0;
			}
		else if (cur_ch == ';')
			{
			skip();
			return quo(parse_form());
			}
		else if (cur_ch == '=')
			{
			skip();
			return app(quo(hold(Qonce)),parse_exp());
			}
		else
			{
			type type = type_E; // eager
			if (cur_ch == '\\')
				{
				skip();
				type = type_D; // direct
				}

			skip_filler();
			return parse_lambda(first_line,type);
			}
		}
	else if (cur_ch == ';')
		{
		skip();
		return parse_exp();
		}
	else
		{
		value factor = parse_term();
		if (cur_ch == '=')
			syntax_error("Missing name declaration before '='", cur_line);
		return factor;
		}
	}

// Parse an expression.
static value parse_exp(void)
	{
	value exp = parse_factor();
	if (exp == 0) return quo(hold(QI));
	while (1)
		{
		value factor = parse_factor();
		if (factor == 0) return exp;
		exp = app(exp,factor);
		}
	}

// Parse a top level form.
static value parse_top(void)
	{
	value exp = parse_form();
	if (cur_ch != -1)
		syntax_error("Extraneous input", cur_line);
	return exp;
	}

static value type_parse_fexl(value f)
	{
	return parse_top();
	(void)f;
	}

// Parse a top level form.
value parse_fexl(value stream, value label)
	{
	cur_label = label;
	return read_stream(stream, Q0(type_parse_fexl));
	}
