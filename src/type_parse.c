#include <input.h>
#include <stdio.h>
#include <value.h>
#include <basic.h>
#include <die.h>
#include <file_input.h>
#include <output.h>
#include <parse.h>
#include <str.h>
#include <str_input.h>
#include <type_parse.h>
#include <type_str.h>
#include <type_sym.h>

static value parse_file(string name)
	{
	value form;
	struct file_input save;

	get_from_file(name->data,&save);
	if (!getd)
		{
		put_to_error();
		put("Could not open source file ");put(name->data);nl();
		die(0);
		}

	form = parse_source(name->data);
	restore_file_input(&save);
	return form;
	}

value type_parse_file(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value form = parse_file(data(x));
		replace_A(f, A(Q(type_resolve),hold(x)), form);
		}
	else
		replace_void(f);

	drop(x);
	return 0;
	}
	}

static value parse_string(string text)
	{
	value form;
	struct str_input save;

	get_from_string(text,&save);
	form = parse_source(0);
	restore_str_input(&save);
	return form;
	}

value type_parse_string(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		value form = parse_string(data(x));
		replace_A(f, A(Q(type_resolve),Qstr(str_new_data0(""))), form);
		}
	else
		replace_void(f);

	drop(x);
	return 0;
	}
	}
