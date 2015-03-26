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
#include <type_get_function.h>
#include <type_str.h>
#include <type_sym.h>

value type_get_function_from_file(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		const char *name = ((string)data(x))->data;
		struct file_input save;

		get_from_file(name,&save);
		if (!getd)
			{
			put_to_error();
			put("Could not open source file ");put(name);nl();
			die(0);
			}

		f = parse_source(name);

		restore_file_input(&save);

		f = A(A(Q(type_resolve),hold(x)),f);
		}
	else
		{
		replace_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}

value type_get_function_from_string(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		string text = (string)data(x);

		struct str_input save;
		get_from_string(text,&save);

		f = parse_source(0);
		restore_str_input(&save);

		f = A(A(Q(type_resolve),Qstr(str_new_data0(""))),f);
		}
	else
		{
		replace_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}
