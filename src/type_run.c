#include <value.h>
#include <basic.h>
#include <die.h>
#include <fexl.h>
#include <num.h>
#include <str.h>
#include <type_num.h>
#include <type_run.h>
#include <type_str.h>

value type_die(value f)
	{
	(void)f;
	die(0);
	return 0;
	}

/* (argv i) Return the command line argument at position i (starting at 0), or
void if no such position. */
value type_argv(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		int i = (int)(*(number)data(x));
		if (i >= 0 && i < main_argc)
			reduce_str(f,str_new_data0(main_argv[i]));
		else
			reduce_void(f);
		}
	else
		reduce_void(f);
	drop(x);
	return 0;
	}
	}
