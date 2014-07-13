#include <value.h>
#include <basic.h>
#include <define.h>
#include <die.h>
#include <output.h>
#include <source.h>
#include <str.h>
#include <type_str.h>
#include <type_output.h>
#include <type_sys.h>

void type_eval(value f)
	{
	string name = get_str(eval(f->R));
	if (name == 0)
		replace_void(f);
	else
		{
		value g = resolve_file(name->data,define_sym);
		replace(f,g);
		drop(g);
		}
	}

void type_die(value f)
	{
	put_to_error();
	putv(eval(f->R)); nl();
	die("");
	}
