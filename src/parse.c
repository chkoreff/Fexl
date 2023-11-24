#include <str.h>
#include <value.h>

#include <basic.h>
#include <buf.h>
#include <parse.h>
#include <report.h>
#include <stream.h>
#include <string.h>
#include <type_num.h>
#include <type_str.h>
#include <type_sym.h>
#include <type_tuple.h>

// Fexl grammar:
// [
// exp     =>  empty
// exp     =>  exp factor
//
// factor  =>  "\;" exp
// factor  =>  "\=" exp
// factor  =>  "\" name def exp      # eager substitution
// factor  =>  "\\" name def exp     # direct substitution
// factor  =>  ";" exp
// factor  =>  term
//
// def     =>  empty
// def     =>  "=" term
//
// term    =>  "(" exp ")"
// term    =>  "[" items "]"
// term    =>  "{" items "}"
// term    =>  string
// term    =>  name
//
// items   =>  empty
// items   =>  term items
// items   =>  ";" exp
//
// string  =>  quote_string
// string  =>  tilde_string
// ]
//
// The parser reads an expression from the input until it reaches end of file
// or the special token "\#" which acts like end of file.  It skips white space
// and comments which appear before syntactic elements.  White space is any
// character with an ASCII value less than or equal to ' ' (SPACE).  A comment
// starts with '#' and continues through end of line.
//
// The "empty" element means end of file.
//
// A "name" is any sequence of characters other than white space or these:
//   \ ( ) [ ] { } ; " ~ # =
//
// A "quote_string" starts with a '"' (QU), followed by any sequence of
// characters other than QU, followed by a closing QU.
//
// A "tilde_string" is a sequence of arbitrary characters enclosed within a
// pair of delimiters of your choice.  It starts with a delimiter, which is a
// '~' (tilde), then zero or more non white characters, then a single white
// space which ends the delimiter and is ignored.  That initial delimiter is
// followed by a sequence of characters which constitute the actual content of
// the string, terminated by a repeat occurrence of the delimiter.

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

	if (!buf.top) return 0;
	return buf_clear(&buf);
	}

static void error_unclosed(unsigned long first_line)
	{
	syntax_error("Unclosed string", first_line);
	}

static struct form parse_quote_string(void)
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

static struct form parse_tilde_string(void)
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

static struct form parse_symbol(void)
	{
	unsigned long first_line = cur_line;
	string name = parse_name();
	if (name == 0) return form_null();
	// See if it's a numeric constant.
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

static struct form parse_term(void);
static struct form parse_exp(void);

static struct form parse_nested(void)
	{
	unsigned long first_line = cur_line;
	struct form exp;
	skip();
	exp = parse_exp();
	if (cur_ch != ')')
		syntax_error("Unclosed parenthesis", first_line);
	skip();
	return exp;
	}

static struct form parse_items(void)
	{
	skip_filler();
	if (cur_ch == ';')
		{
		skip();
		return parse_exp();
		}
	else
		{
		struct form term = parse_term();
		if (term.exp == 0)
			return form_val(hold(Qnull));
		else
			return form_join(type_list,term,parse_items());
		}
	}

static struct form parse_seq(const char t_ch, const char *msg)
	{
	unsigned long first_line = cur_line;
	skip();
	{
	struct form exp = parse_items();
	if (cur_ch != t_ch)
		syntax_error(msg, first_line);
	skip();
	return exp;
	}
	}

static struct form parse_tuple(void)
	{
	struct form list = parse_seq('}',"Unclosed brace");
	value exp = list.exp;
	// If the list is exactly two items change it to type_pair.
	if (1
		&& exp->T == type_list
		&& exp->R->T == type_list
		&& exp->R->R->T == type_null
		)
		{
		struct table *table = list.table;
		if (table)
			{
			unsigned long i;
			for (i = 0; i < table->count; i++)
				{
				struct symbol *sym = &table->vec[i];
				if (sym->pattern->R->T == 0)
					{
					value p = hold(sym->pattern->R->L);
					drop(sym->pattern->R);
					sym->pattern->R = p;
					}
				}
			}
		{
		value R = hold(exp->R->L);
		drop(exp->R);
		exp->R = R;
		exp->T = type_pair;
		}
		return list;
		}
	else
		return form_appv(form_val(hold(Qtuple)),list);
	}

static struct form parse_list(void)
	{
	return parse_seq(']',"Unclosed bracket");
	}

static struct form parse_term(void)
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
static struct form parse_lambda(unsigned long first_line, type type)
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
		return form_lam(type,name,parse_exp()); // No definition

	first_line = cur_line;
	skip();

	// Parse the definition.
	{
	struct form def;
	struct form exp;

	skip_filler();
	def = parse_term();
	if (def.exp == 0)
		syntax_error("Missing definition", first_line);

	exp = form_lam(type,name,parse_exp());
	return form_app(exp,def);
	}
	}

// Parse a form (unresolved symbolic expression).
static value parse_form(void)
	{
	struct form exp = parse_exp();
	exp.label = hold(cur_label);
	return Qform(exp);
	}

// Parse the next factor of an expression, if any.
static struct form parse_factor(void)
	{
	skip_filler();
	if (cur_ch == -1)
		return form_null();
	else if (cur_ch == '\\')
		{
		unsigned long first_line = cur_line;
		skip();
		if (cur_ch == '#')
			{
			cur_ch = -1; // "\#" simulates end of file.
			return form_null();
			}
		else if (cur_ch == ';')
			{
			skip();
			return form_val(parse_form());
			}
		else if (cur_ch == '=')
			{
			skip();
			return form_app(form_val(hold(Qonce)),parse_exp());
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
		struct form factor = parse_term();
		if (cur_ch == '=')
			syntax_error("Missing name declaration before '='", cur_line);
		return factor;
		}
	}

// Parse an expression.
static struct form parse_exp(void)
	{
	struct form exp = parse_factor();
	if (exp.exp == 0) return form_val(hold(QI));
	while (1)
		{
		struct form factor = parse_factor();
		if (factor.exp == 0) return exp;
		exp = form_app(exp,factor);
		}
	}

static value type_parse_fexl(value fun, value f)
	{
	value exp = parse_form();
	if (cur_ch != -1)
		syntax_error("Extraneous input", cur_line);
	(void)fun;
	(void)f;
	return exp;
	}

// Parse a top level form.
value parse_fexl(value stream, value label)
	{
	cur_label = label;
	return read_stream(stream, Q0(type_parse_fexl));
	}
