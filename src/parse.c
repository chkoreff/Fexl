#include <str.h>
#include <value.h>

#include <basic.h>
#include <buffer.h>
#include <parse.h>
#include <report.h>
#include <stream.h>
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>

/*
Grammar:
[
exp    => empty
exp    => term exp
exp    => ; exp
exp    => \ name exp
exp    => \ name = term exp
exp    => \ name == term exp
exp    => \; exp
exp    => \= exp

term   => ( exp )
term   => [ list ]
term   => { tuple }
term   => name
term   => string

list   => empty
list   => term list
list   => term ; exp

tuple  => empty
tuple  => term tuple

string => quote_string
string => tilde_string
]

The Fexl parser reads an expression from the input until it reaches end of file
or the special token "\\" which acts like end of file.
*/

/* Skip to end of line. */
static void skip_line(void)
	{
	while (cur_ch != '\n' && cur_ch != -1)
		skip();
	}

/* Skip white space and comments. */
static void skip_filler(void)
	{
	while (1)
		{
		if (at_white())
			skip();
		else if (cur_ch == '#')
			skip_line();
		else
			return;
		}
	}

/* Collect chars to the first white space.  Return true if found. */
static int collect_to_white(struct buffer *buf)
	{
	while (1)
		{
		if (cur_ch > ' ')
			buf_keep(buf);
		else if (cur_ch == -1)
			return 0;
		else
			{
			skip();
			return 1;
			}
		}
	}

/* Collect chars to t_ch.  Return true if found. */
static int collect_to_ch(struct buffer *buf, const char t_ch)
	{
	while (1)
		{
		if (cur_ch == t_ch)
			{
			skip();
			return 1;
			}
		else if (cur_ch == -1)
			return 0;
		else
			buf_keep(buf);
		}
	}

/* Collect chars to a terminator string.  Return true if found. */
static int collect_string(struct buffer *buf, const char *end,
	unsigned long len)
	{
	if (len == 1) /* Optimize this case. */
		return collect_to_ch(buf,end[0]);
	{
	unsigned long pos = 0;

	while (pos < len)
		{
		if (cur_ch == end[pos])
			{
			pos++;
			skip();
			}
		else if (pos > 0)
			{
			/* Buffer the ones matched so far and start over. */
			buf_addn(buf,end,pos);
			pos = 0;
			}
		else if (cur_ch == -1)
			return 0;
		else
			buf_keep(buf);
		}
	return 1;
	}
	}

/* Collect chars to a white space to get a terminator string.  Then collect
chars to the next occurrence of that terminator.  Return:

	-1 : if no terminator was found
	 0 : if no occurrence of the terminator was found
	 1 : if the terminator was found
*/
static int collect_tilde_string(struct buffer *buf)
	{
	if (!collect_to_white(buf)) return -1;
	{
	string end = buf_clear(buf);
	int ok = collect_string(buf,end->data,end->len);
	str_free(end);
	return ok;
	}
	}

static value cur_label; /* label of current input stream */

static void syntax_error(const char *code, unsigned long line)
	{
	fatal_error(code,line,str_data(cur_label));
	}

/* Parse a name, or return 0 if I don't see one.

A name may contain just about anything, except for white space and a few other
special chars.  This is the simplest rule that can work.
*/
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

	if (!buf.top) return 0;
	return buf_clear(&buf);
	}

static void error_unclosed(unsigned long first_line)
	{
	syntax_error("Unclosed string", first_line);
	}

static struct form *parse_quote_string(void)
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

	return form_val(Qstr(buf_clear(&s_buf)));
	}

static struct form *parse_tilde_string(void)
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

	return form_val(Qstr(buf_clear(&buf)));
	}

static struct form *parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return 0;
	/* See if it's a numeric constant. */
	{
	value n = Qnum_str0(name->data);
	if (n)
		{
		str_free(name);
		return form_val(n);
		}
	else
		return form_ref(name,first_line);
	}
	}

static struct form *parse_term(void);
static struct form *parse_exp(void);

static struct form *parse_nested(void)
	{
	unsigned long first_line = cur_line;
	struct form *exp;
	skip();
	exp = parse_exp();
	if (cur_ch != ')')
		syntax_error("Unclosed parenthesis", first_line);
	skip();
	return exp;
	}

/* LATER 20220811 Unify list/tuple representation and reduce this code. */
static struct form *parse_list_items(void)
	{
	struct form *term = parse_term();
	if (term == 0) return form_val(hold(Qnull));
	skip_filler();
	if (cur_ch == ';')
		return form_join(0,term,parse_exp());
	else
		return form_join(0,term,parse_list_items());
	}

static struct form *parse_tuple_items(void)
	{
	struct form *term = parse_term();
	if (term == 0) return form_val(hold(Qnull));
	skip_filler();
	return form_join(0,term,parse_tuple_items());
	}

static struct form *parse_list(void)
	{
	unsigned long first_line = cur_line;
	struct form *exp;
	skip();
	skip_filler();
	exp = parse_list_items();
	if (exp->exp->T == 0)
		exp = form_appv(form_val(hold(Qlist)),exp);
	if (cur_ch != ']')
		syntax_error("Unclosed bracket", first_line);
	skip();
	return exp;
	}

static struct form *parse_tuple(void)
	{
	struct form *exp;
	unsigned long first_line = cur_line;
	skip();
	skip_filler();
	exp = form_appv(form_val(hold(Qtuple)),parse_tuple_items());
	if (cur_ch != '}')
		syntax_error("Unclosed brace", first_line);
	skip();
	return exp;
	}

static struct form *parse_term(void)
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

/* Parse a lambda form following the initial '\' character. */
static struct form *parse_lambda(unsigned long first_line)
	{
	string name = parse_name();
	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	/* Lambda name cannot be a numeric constant. */
	{
	value n = Qnum_str0(name->data);
	if (n)
		syntax_error("Lambda name cannot be a number", first_line);
	}

	skip_filler();
	if (cur_ch != '=')
		return form_lam(name,parse_exp()); /* No definition */

	first_line = cur_line;
	skip();

	/* Parse the definition. */
	{
	int eager = 0;
	struct form *def;
	struct form *exp;

	if (cur_ch == '=')
		skip();
	else
		eager = 1;

	skip_filler();
	def = parse_term();
	if (def == 0)
		syntax_error("Missing definition", first_line);

	exp = form_lam(name,parse_exp());
	if (eager)
		return form_appv(form_appv(form_val(hold(Qeval)),def),exp);
	else
		return form_appv(exp,def);
	}
	}

/* Parse a form (unresolved symbolic expression). */
static value parse_form(void)
	{
	struct form *exp = parse_exp();
	exp->label = hold(cur_label);
	return Qform(exp);
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static struct form *parse_factor(void)
	{
	skip_filler();
	if (cur_ch == -1)
		return 0;
	else if (cur_ch == '\\')
		{
		unsigned long first_line = cur_line;
		skip();
		if (cur_ch == '\\')
			{
			cur_ch = -1; /* Two backslashes simulates end of file. */
			return 0;
			}
		else if (cur_ch == ';')
			{
			skip();
			return form_val(parse_form());
			}
		else if (cur_ch == '=')
			{
			skip();
			return form_appv(form_val(hold(Qonce)),parse_exp());
			}
		else
			{
			skip_filler();
			return parse_lambda(first_line);
			}
		}
	else if (cur_ch == ';')
		{
		skip();
		return parse_exp();
		}
	else
		{
		struct form *factor = parse_term();
		if (cur_ch == '=')
			syntax_error("Missing name declaration before '='", cur_line);
		return factor;
		}
	}

/* Parse an expression. */
static struct form *parse_exp(void)
	{
	struct form *exp = parse_factor();
	if (exp == 0) return form_val(hold(QI));
	while (1)
		{
		struct form *factor = parse_factor();
		if (factor == 0) return exp;
		exp = form_app(exp,factor);
		}
	}

static value type_parse_fexl(value f)
	{
	value exp = parse_form();
	if (cur_ch != -1)
		syntax_error("Extraneous input", cur_line);
	(void)f;
	return exp;
	}

/* Parse a top level form. */
value parse_fexl(value stream, value label)
	{
	cur_label = label;
	return read_stream(stream,V(type_parse_fexl,0,0));
	}
