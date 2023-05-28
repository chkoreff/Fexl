#include <value.h>

#include <basic.h>
#include <context.h>
#include <memory.h>
#include <show.h>
#include <stdio.h>

#include <type_meta.h>

// Benchmark version which counts eval calls.
static unsigned long cur_steps = 0;

static value count_step_A(value f)
	{
	cur_steps++;
	return step_A(f);
	}

static value op_benchmark(value f, value x, FILE *fh)
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

	fprintf(fh,"steps %lu bytes %lu\n",cur_steps,num_bytes);
	cur_steps += beg_steps;
	drop(f);
	return x;
	}
	}
	}

static value apply_show_benchmark(value f, value x)
	{
	return op_benchmark(f,x,stdout);
	}

static value apply_trace_benchmark(value f, value x)
	{
	return op_benchmark(f,x,stderr);
	}

static struct type type_show_benchmark =
	{ 0, apply_show_benchmark, no_clear };
static struct type type_trace_benchmark =
	{ 0, apply_trace_benchmark, no_clear };

static value apply_show(value f, value x)
	{
	show(x);
	drop(f);
	drop(x);
	return hold(QI);
	}

static struct type type_show = { 0, apply_show, no_clear };

void def_meta(void)
	{
	// LATER introspection functions
	define("show_benchmark", Q(&type_show_benchmark,0));
	define("trace_benchmark", Q(&type_trace_benchmark,0));
	define("show", Q(&type_show,0));
	}
