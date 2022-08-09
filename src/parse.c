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

static value cur_label; /* label of current input stream */

static void syntax_error(const char *code, unsigned long line)
	{
	fatal_error(code,line,str_data(cur_label));
	}

/* Parse a name, or return 0 if I don't see one.

A name may contain just about anything, except for white space and a few other
special characters.  This is the simplest rule that can work.
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
		if (cur_ch == end_data[match_pos])
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
		else if (cur_ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			buf_keep(&buf);
		}

	return buf_clear(&buf);
	}

/* Parse a tilde string terminator. */
static string parse_terminator(unsigned long first_line)
	{
	struct buffer buf = {0};
	while (cur_ch > ' ')
		buf_keep(&buf);

	if (cur_ch == -1)
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

static struct form *parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	if (cur_ch == '"')
		{
		skip();
		return form_val(Qstr(collect_string("\"",1,first_line)));
		}
	else if (cur_ch == '~')
		{
		string end = parse_terminator(first_line);
		string content = parse_content(end,first_line);
		return form_val(Qstr(content));
		}
	else
		{
		string name = parse_name();
		if (name == 0) return 0;
		/* See if it's a numeric constant. */
		{
		value def = Qnum_str0(name->data);
		if (def)
			{
			str_free(name);
			return form_val(def);
			}
		else
			return form_ref(name,first_line);
		}
		}
	}

static struct form *parse_term(void);
static struct form *parse_exp(void);

static struct form *parse_list(void)
	{
	struct form *term = parse_term();
	if (term == 0) return form_val(hold(Qnull));
	skip_filler();
	return form_join(0,term, (cur_ch == ';' ? parse_exp() : parse_list()));
	}

static struct form *parse_tuple(void)
	{
	struct form *term = parse_term();
	if (term == 0) return form_val(hold(Qnull));
	skip_filler();
	return form_join(0,term,parse_tuple());
	}

static struct form *parse_term(void)
	{
	struct form *exp;
	unsigned long first_line = cur_line;
	if (cur_ch == '(') /* parenthesized expression */
		{
		skip();
		exp = parse_exp();
		if (cur_ch != ')')
			syntax_error("Unclosed parenthesis", first_line);
		skip();
		}
	else if (cur_ch == '[') /* list */
		{
		skip();
		skip_filler();
		exp = parse_list();
		if (exp->exp->T == 0)
			exp = form_appv(form_val(hold(Qlist)),exp);
		if (cur_ch != ']')
			syntax_error("Unclosed bracket", first_line);
		skip();
		}
	else if (cur_ch == '{') /* tuple */
		{
		skip();
		skip_filler();
		exp = form_appv(form_val(hold(Qtuple)),parse_tuple());
		if (cur_ch != '}')
			syntax_error("Unclosed brace", first_line);
		skip();
		}
	else
		exp = parse_symbol();

	return exp;
	}

/* Parse a lambda form following the initial '\' character. */
static struct form *parse_lambda(unsigned long first_line)
	{
	string name;
	struct form *def = 0;
	struct form *exp;
	int eager = 1;

	/* Parse the name. */
	skip_white();
	name = parse_name();

	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	/* Lambda name cannot be a numeric constant. */
	{
	value def = Qnum_str0(name->data);
	if (def)
		syntax_error("Lambda name cannot be a number", first_line);
	}

	/* Parse the optional definition of the name. */
	skip_filler();
	first_line = cur_line;
	if (cur_ch == '=')
		{
		skip();
		if (cur_ch == '=')
			{
			eager = 0;
			skip();
			}
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function and abstract the name out. */
	exp = form_lam(name,parse_exp());

	/* Apply the definition if any. */
	if (def == 0)
		return exp;
	else if (eager)
		return form_appv(form_appv(form_val(hold(Qeval)),def),exp);
	else
		return form_app(exp,def);
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
			return parse_lambda(first_line);
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
