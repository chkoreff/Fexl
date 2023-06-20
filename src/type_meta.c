#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <parse.h>
#include <show.h>
#include <type_record.h>
#include <type_str.h>

#include <type_meta.h>

static value op_benchmark(value fun, value arg, FILE *fh)
	{
	clear_free_list();
	{
	unsigned long beg_steps = cur_steps;
	unsigned long beg_bytes = cur_bytes;

	cur_steps = 0;
	arg = eval(arg);

	{
	long num_bytes = cur_bytes - beg_bytes;
	fprintf(fh,"steps %lu bytes %ld\n",cur_steps,num_bytes);
	cur_steps += beg_steps;
	drop(fun);
	return arg;
	}
	}
	}

value type_show_benchmark(value fun, value arg)
	{
	return op_benchmark(fun,arg,stdout);
	}

value type_trace_benchmark(value fun, value arg)
	{
	return op_benchmark(fun,arg,stderr);
	}

value type_show(value fun, value arg)
	{
	(void)fun;
	show(arg);
	return type_F(fun,arg);
	}

value type_read(value fun, value arg)
	{
	arg = eval(arg);
	if (arg->T == type_str)
		{
		const char *name = str_data(arg);
		FILE *fh = open_source(name);
		value exp = parse_fh(arg,fh);
		drop(fun);
		return exp;
		}
	else
		return type_void(fun,arg);
	}
