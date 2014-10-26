#include <str.h>
#include <value.h>
#include <basic.h>
#include <die.h>
#include <file_input.h>
#include <input.h>
#include <output.h>
#include <parse.h>
#include <source.h>
#include <str_input.h>
#include <type_str.h>

static const char *source_label;

static void put_loc(unsigned long line)
	{
	put(" on line "); put_ulong(line);
	if (source_label)
		{
		put(" of ");put(source_label);
		}
	nl();
	}

void syntax_error(const char *code, unsigned long line)
	{
	put_to_error();
	put(code); put_loc(line);
	die(0);
	}

void undefined_symbol(const char *name, unsigned long line)
	{
	put_to_error();
	put("Undefined symbol "); put(name); put_loc(line);
	}

value eval_file(const char *name)
	{
	value exp;
	input save_getd = getd;
	const char *save_source_label = source_label;

	get_from_file(name);
	source_label = name;
	exp = get_function();

	getd = save_getd;
	source_label = save_source_label;
	return eval(exp);
	}

static value eval_str(string x)
	{
	value exp;
	input save_getd = getd;

	get_from_string(x);
	exp = get_function();

	getd = save_getd;
	return eval(exp);
	}

static value eval_str0(const char *s)
	{
	string x = str_new_data0(s);
	value exp = eval_str(x);
	str_free(x);
	return exp;
	}

/*
# LATER get_from_file
# LATER get_from_string
# LATER get_from_input
# LATER get_function

(
\(source_context)
get_from_file file_name
\source_name=file_name
get_function source_name  # source_name used for error messages
)

(
\(source_context)
get_from_string source_string
get_function source_name  # source_name used for error messages
)

\(source_context)
get_from_input  # stdin
get_function source_name  # source_name used for error messages
*/

/*LATER eval any input stream (file, string, etc.) */
/*LATER use arbitrary context, not just the standard one */

/*LATER after we do get_function in fexl, we'll no longer need eval_file
and eval_str defined in C.  We can define those in fexl easily.

\eval_file=
	(\name
	get_from_file name ;
	\source_name=name
	get_function source_name
	)

\eval_str=
	(\text
	get_from_string text ;
	\source_name=""
	get_function source_name
	)
*/
value type_eval_file(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		const char *name = ((string)x->R)->data;
		f = eval_file(name);
		}
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

value type_eval_str(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		const char *name = ((string)x->R)->data;
		f = eval_str0(name);
		}
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

