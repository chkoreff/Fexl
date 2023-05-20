#include <die.h>
#include <stdio.h>

#include <str.h>
#include <io.h>
#include <buffer.h>
#include <stream.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <basic.h>
#include <type_str.h>

#include <parse.h>

static value cx_env;
static value cx_lam;
static int has_undef = 0;

static void syntax_error(const char *code, unsigned long line)
	{
	fprintf(stderr,"%s on line %lu", code, line);
	if (cur_name[0])
		fprintf(stderr," of %s\n", cur_name);
	else
		fprintf(stderr,"\n");
	die(0);
	}

static void undefined_symbol(const char *name, unsigned long line)
	{
	fprintf(stderr,"Undefined symbol %s on line %lu", name, line);
	if (cur_name[0])
		fprintf(stderr," of %s\n", cur_name);
	else
		fprintf(stderr,"\n");
	has_undef = 1;
	}

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

static value parse_list(void)
	{
	syntax_error("TODO parse_list", cur_line);
	return 0;
	}

static value parse_tuple(void)
	{
	syntax_error("TODO parse_tuple", cur_line);
	return 0;
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

	return Qstr(buf_clear(&s_buf));
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

	return Qstr(buf_clear(&buf));
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

	if (!buf.top) return 0;
	return buf_clear(&buf);
	}

static value find_sym(string name)
	{
	value cx = cx_lam;
	unsigned long pos = 0;
	while (1)
		{
		if (cx->T == &type_ref)
			return 0;
		else if (str_eq(name, cx->L->v_ptr))
			return R(pos);
		pos++;
		cx = cx->R;
		}
	}

static value find_env(string name)
	{
	value cx = cx_env;
	while (1)
		{
		if (cx->T == &type_ref)
			return 0;
		{
		value top = cx->L;
		value top_fun = top->L;
		string top_name = top_fun->v_ptr;

		if (str_eq(name,top_name))
			return hold(top->R);
		cx = cx->R;
		}
		}
	}

static value parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return 0;
	// TODO See if it's a numeric constant.

	{
	value exp = find_sym(name);
	if (exp == 0)
		{
		exp = find_env(name);
		if (exp == 0)
			{
			undefined_symbol(name->data,first_line);
			exp = hold(QI);
			}
		}

	str_free(name);
	return exp;
	}
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

static value parse_def(void)
	{
	value def;
	if (cur_ch == '=')
		{
		unsigned long first_line = cur_line;
		skip();
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}
	else
		def = 0;
	return def;
	}

static value parse_lambda(unsigned long first_line)
	{
	string name = parse_name();
	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	skip_filler();
	{
	value def = parse_def();
	value old_cx = hold(cx_lam);

	cx_lam = A(Qstr(name),cx_lam);
	{
	value exp = parse_exp();
	drop(cx_lam);
	cx_lam = old_cx;

	exp = L(exp);
	if (def)
		exp = A(exp,def);
	return exp;
	}
	}
	}

// Parse the next factor of an expression, if any.
static value parse_factor(void)
	{
	skip_filler();
	if (cur_ch == EOF)
		return 0;
	else if (cur_ch == '\\')
		{
		unsigned long first_line = cur_line;
		skip();
		if (cur_ch == '#')
			{
			cur_ch = EOF; // "\#" simulates end of file.
			return 0;
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
	if (exp == 0)
		return hold(QI);
	while (1)
		{
		value factor = parse_factor();
		if (factor == 0) return exp;
		exp = A(exp,factor);
		}
	}

// Parse a top level expression.
value parse_fexl(void)
	{
	// Define standard context.
	// TODO implement context as a function.
	cx_env = R(0);
	cx_env = A(A(Qstr(str_new_data0("I")),hold(QI)),cx_env);
	cx_env = A(A(Qstr(str_new_data0("void")),hold(Qvoid)),cx_env);

	cx_lam = R(0); // empty context
	has_undef = 0;

	{
	value exp = parse_exp();
	if (cur_ch != EOF)
		syntax_error("Extraneous input", cur_line);
	if (has_undef)
		die(0);

	drop(cx_lam);
	drop(cx_env);
	return A(exp,R(0));
	}
	}
