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
	drop(f);
	return x;
	}
	}
	}

static struct type type_benchmark = { 0, apply_benchmark, no_clear };

static value apply_value_I(value f, value x)
	{
	void (*op)(value) = f->v_ptr;
	op(x);
	drop(f);
	drop(x);
	return hold(QI);
	}

static struct type type_value_I = { 0, apply_value_I, no_clear };

static value apply_str_value(value f, value x)
	{
	value g;
	x = eval(x);
	if (x->T == &type_str)
		{
		value (*op)(string) = f->v_ptr;
		g = op(x->v_ptr);
		}
	else
		g = hold(Qvoid);

	drop(f);
	drop(x);
	return g;
	}

static struct type type_str_value = { 0, apply_str_value, no_clear };

static value load(string s)
	{
	const char *name = s->data;
	FILE *fh = open_source(name);
	return load_fh(name,fh);
	}

void use_meta(void)
	{
	// LATER introspection functions
	define("show_benchmark", Q(&type_benchmark,stdout));
	define("trace_benchmark", Q(&type_benchmark,stderr));
	define("show", Q(&type_value_I,show));
	define("load", Q(&type_str_value,load));
	}
