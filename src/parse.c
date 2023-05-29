#include <str.h>

#include <buffer.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <die.h>
#include <stdio.h>
#include <stream.h>
#include <type_num.h>
#include <type_str.h>

#include <parse.h>

static value cx_lam;

static value pre(value x)
	{
	return A(hold(QI),x);
	}

static value ref(value sym)
	{
	return A(A(A(sym,hold(QT)),hold(QI)),hold(QI));
	}

static value merge(value fun, value arg)
	{
	int cmp;
	if (fun->T == &type_A)
		{
		if (arg->T == &type_A)
			cmp = str_cmp(fun->L->L->v_ptr, arg->L->L->v_ptr);
		else
			cmp = -1;
		}
	else
		{
		if (arg->T != &type_A)
			return hold(arg);
		cmp = 1;
		}

	if (cmp == 0)
		return
			A(A(hold(fun->L->L), A(hold(fun->L->R),hold(arg->L->R))),
			merge(fun->R,arg->R)
			);
	else if (cmp < 0)
		return
			A(A(hold(fun->L->L), A(hold(fun->L->R),hold(QF))),
			merge(fun->R,arg)
			);
	else
		return
			A(A(hold(arg->L->L), A(hold(QF),hold(arg->L->R))),
			merge(fun,arg->R)
			);
	}

static value app(value fun, value arg)
	{
	value exp = A(merge(fun->L,arg->L), A(hold(fun->R),hold(arg->R)));
	drop(fun);
	drop(arg);
	return exp;
	}

static void put_error_location(unsigned long line)
	{
	fprintf(stderr," on line %lu", line);
	if (cur_name[0])
		fprintf(stderr," of %s\n", cur_name);
	else
		fprintf(stderr,"\n");
	}

static void syntax_error(const char *code, unsigned long line)
	{
	fprintf(stderr,"%s",code);
	put_error_location(line);
	die(0);
	}

static void undefined_symbol(const char *name, unsigned long line)
	{
	fprintf(stderr,"Undefined symbol %s",name);
	put_error_location(line);
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
	syntax_error("LATER parse_list", cur_line);
	return 0;
	}

static value parse_tuple(void)
	{
	syntax_error("LATER parse_tuple", cur_line);
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

	return pre(Qstr(buf_clear(&s_buf)));
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

	return pre(Qstr(buf_clear(&buf)));
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

static value find_item(string name, value cx)
	{
	while (1)
		{
		if (cx->T == &type_A)
			{
			value top = cx->L;
			if (str_eq(name, top->L->v_ptr))
				return top;
			else
				cx = cx->R;
			}
		else
			return 0;
		}
	}

static value resolve(string name)
	{
	value exp = find_item(name, cx_lam);
	if (exp)
		return hold(exp->R);

	exp = find_item(name, cx_std);
	if (exp)
		return pre(hold(exp->R));

	exp = Qnum_str0(name->data); // Check for numeric constant.
	if (exp)
		return pre(exp);

	return 0;
	}

static value parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return 0;
	{
	value exp = resolve(name);
	if (exp == 0)
		{
		undefined_symbol(name->data,first_line);
		exp = ref(Qstr(name));
		}
	else
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
	value def = 0;
	if (cur_ch == '=')
		{
		unsigned long first_line = cur_line;
		skip();
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}
	return def;
	}

// Pop the name out of the map, returning A(pattern,new_map).
static value pop(string name, value map)
	{
	if (map->T == &type_A)
		{
		value remain = pop(name,map->R);
		value pattern = hold(remain->L);
		value tail = hold(remain->R);
		value top = map->L;
		value top_sym = top->L;
		value top_pattern = hold(top->R);

		drop(remain);

		if (pattern == QF && str_eq(name,top_sym->v_ptr))
			{
			drop(pattern);
			return A(top_pattern, tail); // found
			}
		else
			return A(pattern,
				A(A(hold(top_sym),A(hold(QF),top_pattern)), tail));
		}
	else
		return A(hold(QF),hold(map));
	}

static value abstract(string name, value exp, type type)
	{
	value remain = pop(name,exp->L);
	value pattern = hold(remain->L);
	value new_map = hold(remain->R);
	value form = hold(exp->R);

	drop(remain);
	drop(exp);
	return A(new_map,V(type,pattern,form));
	}

static value parse_lambda(unsigned long first_line, type type)
	{
	string name = parse_name();
	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	skip_filler();
	{
	value def = parse_def();
	value save_cx = hold(cx_lam);
	value sym = Qstr(name);
	cx_lam = A(A(sym,ref(hold(sym))),cx_lam);

	{
	value exp = parse_exp();
	exp = abstract(name,exp,type);

	drop(cx_lam);
	cx_lam = save_cx;

	if (def)
		exp = app(exp,def);

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
			type type = &type_L; // lazy
			if (cur_ch == '\\')
				{
				skip();
				type = &type_E; // eager
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
	if (exp == 0)
		return pre(hold(QI));
	while (1)
		{
		value factor = parse_factor();
		if (factor == 0) return exp;
		exp = app(exp,factor);
		}
	}

// Parse a top level expression.
static value parse_fexl(const char *name, input get)
	{
	cx_lam = hold(QI); // empty stack
	cur_name = name;
	cur_get = get;
	skip(); // Read first char.

	{
	value exp = parse_exp();
	value val = hold(exp->R);

	if (cur_ch != EOF)
		syntax_error("Extraneous input", cur_line);

	if (exp->L->T == &type_A)
		die(0); // Has undefined symbols

	drop(exp);
	drop(cx_lam);
	return val;
	}
	}

// Parse a file.

static FILE *cur_fh;

static int get_fh(void)
	{
	//return '('; // Test stack overflow in parser.
	return fgetc(cur_fh);
	}

FILE *open_source(const char *name)
	{
	FILE *fh = name[0] ? fopen(name,"r") : stdin;
	if (!fh)
		{
		fprintf(stderr,"Could not open source file %s\n",name);
		die(0);
		}
	return fh;
	}

value load_fh(const char *name, FILE *fh)
	{
	value g;
	cur_fh = fh;
	g = parse_fexl(name,get_fh);
	fclose(fh);
	return g;
	}
