#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <memory.h>
#include <parse.h>
#include <show.h>
#include <type_record.h>
#include <type_str.h>

#include <die.h> // TODO
#include <output.h> // TODO

#include <type_meta.h>

// Benchmark version which counts eval calls.
static unsigned long cur_steps = 0;

static value count_step_A(value f)
	{
	cur_steps++;
	return step_A(f);
	}

static value apply_benchmark(value f, value x)
	{
	clear_free_list();

	{
	unsigned long beg_steps = cur_steps;
	unsigned long beg_bytes = cur_bytes;

	cur_steps = 0;

	type_A.step = count_step_A;
	x = eval(x);
	type_A.step = step_A;

	{
	unsigned long num_bytes = cur_bytes - beg_bytes;
	FILE *fh = f->v_ptr;

	fprintf(fh,"steps %lu bytes %lu\n",cur_steps,num_bytes);
	cur_steps += beg_steps;
	return x;
	}
	}
	}

static struct type type_benchmark = { 0, apply_benchmark, no_clear };

static value apply_show(value f, value x)
	{
	if (f->L == 0)
		{
		x = eval(x);
		if (x->T == &type_record)
			return V(f->T,hold(f),x);
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	else
		{
		show_in(f->R,x);
		drop(x);
		return hold(QI);
		}
	}

static struct type type_show = { 0, apply_show, clear_T };

static value apply_read(value f, value x)
	{
	(void)f;
	x = eval(x);
	if (x->T == &type_str)
		{
		const char *name = str_data(x);
		FILE *fh = open_source(name);
		value exp = parse_fexl_fh(x,fh);
		exp = V(&type_form,hold(x),exp);
		drop(x);
		return exp;
		}
	else
		{
		drop(x);
		return hold(Qvoid);
		}
	}

static struct type type_read = { 0, apply_read, no_clear }; // TODO move to parse

void use_meta(void)
	{
	// LATER introspection functions
	// LATER die
	define("show_benchmark", Q(&type_benchmark,stdout));
	define("trace_benchmark", Q(&type_benchmark,stderr));
	define("show", Q(&type_show,0));
	define("read", Q(&type_read,0));
	}
