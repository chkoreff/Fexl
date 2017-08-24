#include <num.h>
#include <str.h>
#include <value.h>

#include <die.h>
#include <standard.h>
#include <type_num.h>
#include <type_run.h>
#include <type_str.h>

int main_argc;
char **main_argv;

value type_die(value f)
	{
	die(0);
	return f;
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
		int i = get_double(x);
		if (i >= 0 && i < main_argc)
			f = Qstr(str_new_data0(main_argv[i]));
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
