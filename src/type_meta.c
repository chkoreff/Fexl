#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <memory.h>
#include <parse.h>
#include <show.h>
#include <type_str.h>

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
	(void)f;
	show(x);
	drop(x);
	return hold(QI);
	}

static struct type type_show = { 0, apply_show, no_clear };

static value apply_load(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_str)
		{
		const char *name = str_data(x);
		FILE *fh = open_source(name);
		f = load_fh(name,fh);
		drop(x);
		return f;
		}
	else
		{
		drop(x);
		return hold(Qvoid);
		}
	}

static struct type type_load = { 0, apply_load, no_clear };

void use_meta(void)
	{
	// LATER introspection functions
	// LATER die
	define("show_benchmark", Q(&type_benchmark,stdout));
	define("trace_benchmark", Q(&type_benchmark,stderr));
	define("show", Q(&type_show,0));
	define("load", Q(&type_load,0));
	}
