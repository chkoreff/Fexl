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

/*
Fexl grammar:
[
exp     =>  empty
exp     =>  exp factor

factor  =>  "\" lambda
factor  =>  "\\" lambda
factor  =>  "\;" exp
factor  =>  ";" exp
factor  =>  term

lambda  =>  name def exp

def     =>  empty
def     =>  "=" term

term    =>  "(" exp ")"
term    =>  "[" items "]"
term    =>  "{" items "}"
term    =>  name
term    =>  string

items   =>  empty
items   =>  term items
items   =>  ";" exp

string  =>  quote_string
string  =>  tilde_string
]

The parser reads an expression from the input until it reaches end of file or
the special token "\#" which acts like end of file.  It skips white space and
comments which appear before syntactic elements.  White space is any character
with an ASCII value less than ' ' (SPACE).  A comment starts with '#' and
continues through end of line.

The "empty" element means end of file.

A "name" is any sequence of characters other than white space or these:
	\ ( ) [ ] { } ; " ~ # =

A "quote_string" starts with a '"' (QU), followed by any sequence of characters
other than QU, followed by a closing QU.

A "tilde_string" is a sequence of arbitrary characters enclosed within a pair
of delimiters of your choice.  It starts with a delimiter, which is a '~'
(tilde), then zero or more non white characters, then a single white space
which ends the delimter and is ignored.  That initial delimiter is followed by
a sequence of characters which constitute the actual content of the string,
terminated by a repeat occurrence of the delimiter.
*/

static value cur_label;
static value cx_lam;

// Make an expression for a predefined value.
static value pre(value x)
	{
	return A(hold(QI),x);
	}

// Make an expression which refers to a name on a given line.
static value ref(value name, unsigned long line)
	{
	return A(A(A(A(name,Qnum(line)),hold(QT)),hold(QI)),hold(QI));
	}

static value merge(value fun, value arg)
	{
	int cmp;
	if (fun->T == &type_A)
		{
		if (arg->T == &type_A)
			cmp = str_cmp(fun->L->L->L->v_ptr, arg->L->L->L->v_ptr);
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

static value appv(type t, value fun, value arg)
	{
	value exp = A(merge(fun->L,arg->L), V(t,hold(fun->R),hold(arg->R)));
	drop(fun);
	drop(arg);
	return exp;
	}

static value app(value fun, value arg)
	{
	return appv(&type_A,fun,arg);
	}

static value cons(value fun, value arg)
	{
	return appv(&type_list,fun,arg);
	}

static value tuple(value exp)
	{
	return appv(&type_tuple,pre(hold(QI)),exp);
	}

static void put_error_location(unsigned long line, const char *label)
	{
	fprintf(stderr," on line %lu", line);
	if (label[0])
		fprintf(stderr," of %s\n", label);
	else
		fprintf(stderr,"\n");
	}

static void syntax_error(const char *code, unsigned long line)
	{
	fprintf(stderr,"%s",code);
	put_error_location(line,str_data(cur_label));
	die(0);
	}

static void undefined_symbol(const char *name, unsigned long line,
	const char *label)
	{
	fprintf(stderr,"Undefined symbol %s",name);
	put_error_location(line,label);
	}

static value parse_term(void);
static value parse_exp(void);

static value parse_nested(void)
	{
	unsigned long first_line = cur_line;
	skip();
	{
	value exp = parse_exp();
	if (cur_ch != ')')
		syntax_error("Unclosed parenthesis", first_line);
	skip();
	return exp;
	}
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
			return pre(hold(Qnull));
		else
			return cons(term,parse_items());
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

value find_item(string name, value cx)
	{
	while (1)
		{
		if (cx->T == &type_A)
			{
			value top = cx->L;
			if (str_eq(name, top->L->v_ptr))
				return hold(top->R);
			else
				cx = cx->R;
			}
		else
			return 0;
		}
	}

static value parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return 0;
	{
	value exp = find_item(name, cx_lam); // Check for lambda symbol.
	if (exp == 0)
		{
		exp = Qnum_str0(name->data); // Check for numeric constant.
		if (exp) exp = pre(exp);
		}

	if (exp == 0)
		exp = ref(Qstr(name),first_line); // external symbol
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
		return parse_seq(']',"Unclosed bracket");
	else if (cur_ch == '{')
		return tuple(parse_seq('}',"Unclosed brace"));
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

		drop(remain);

		if (pattern == QF && str_eq(name, map->L->L->L->v_ptr))
			{
			drop(pattern);
			return A(hold(map->L->R), tail); // found
			}
		else
			return A(pattern,
				A(A(hold(map->L->L),A(hold(QF),hold(map->L->R))), tail));
		}
	else
		return A(hold(QF),hold(map));
	}

static value abstract(string name, value exp, type type)
	{
	value remain = pop(name,exp->L);
	value pattern = hold(remain->L);
	value new_map = hold(remain->R);
	value val = hold(exp->R);

	drop(remain);
	drop(exp);
	return A(new_map,V(type,pattern,val));
	}

// Parse an expression in a new temporary lambda context.
static value parse_scope(value cx_new)
	{
	value cx_save = cx_lam;
	cx_lam = cx_new;
	{
	value exp = parse_exp();
	drop(cx_lam);
	cx_lam = cx_save;
	return exp;
	}
	}

static value parse_lambda(unsigned long first_line, type type)
	{
	string name = parse_name();
	if (name == 0)
		syntax_error("Missing name after '\\'", first_line);

	skip_filler();
	{
	value def = parse_def();
	value sym = Qstr(name);
	value exp = parse_scope(A(A(sym,ref(hold(sym),first_line)),hold(cx_lam)));

	exp = abstract(name,exp,type);
	if (def)
		exp = app(exp,def);

	return exp;
	}
	}

// Resolve all external symbols in an expression using the current context.

static value resolve_exp(value label, value exp)
	{
	value val = hold(exp->R);
	value map = exp->L;
	int has_undef = 0;

	while (map->T == &type_A)
		{
		value item = map->L;
		value sym = item->L;
		string name = sym->L->v_ptr;
		value def = find_item(name,cx_cur);

		if (def == 0)
			{
			undefined_symbol(name->data, sym->R->v_double, str_data(label));
			has_undef = 1;
			}
		else
			{
			value new = subst(item->R,val,def);
			drop(def);
			drop(val);
			val = new;
			}
		map = map->R;
		}

	if (has_undef)
		die(0); // Has undefined symbols

	drop(label);
	drop(exp);
	return val;
	}

static value step_form(value f)
	{
	value g = resolve_exp(hold(f->L), hold(f->R));
	drop(f);
	return g;
	}

struct type type_form = { step_form, 0, clear_A };

static value parse_form(void)
	{
	return pre(V(&type_form,hold(cur_label),parse_scope(hold(QI))));
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
		else if (cur_ch == ';')
			{
			skip();
			return parse_form();
			}
		else
			{
			type type = &type_D; // direct
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
static value parse_top(void)
	{
	value exp = parse_scope(hold(QI));

	if (cur_ch != EOF)
		syntax_error("Extraneous input", cur_line);
	return exp;
	}

// Parse a Fexl stream.
static value parse_fexl(value label, input get)
	{
	const int save_ch = cur_ch;
	const unsigned long save_line = cur_line;
	const input save_get = cur_get;

	cur_get = get;
	cur_line = 1;
	skip(); // Read first char.
	cur_label = label;

	{
	value exp = parse_top();

	cur_get = save_get;
	cur_line = save_line;
	cur_ch = save_ch;
	cur_label = 0;

	return exp;
	}
	}

// Parse a Fexl file.

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

static value parse_fexl_fh(value label, FILE *fh)
	{
	cur_fh = fh;
	{
	value val = parse_fexl(label,get_fh);
	fclose(fh);
	cur_fh = 0;
	return val;
	}
	}

value load(value label, FILE *fh)
	{
	return resolve_exp(label, parse_fexl_fh(label,fh));
	}

value loadf(value label, FILE *fh)
	{
	return single(V(&type_form,label,parse_fexl_fh(label,fh)));
	}
