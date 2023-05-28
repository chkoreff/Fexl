#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <type_str.h>

#include <type_output.h>

static value apply_str(value f, value x)
	{
	value g;

	x = eval(x);
	if (x->T == &type_str)
		{
		void (*op)(string) = f->v_ptr;
		op(x->v_ptr);
		}
	g = hold(QI);

	drop(f);
	drop(x);
	return g;
	}

static struct type op_str = { 0, apply_str, no_clear };

static void def1(const char *name, void op(string))
	{
	define(name, Q(&op_str,op));
	}

static value op_nl(void)
	{
	nl();
	return hold(QI);
	}

static void say_str(string x)
	{
	put_str(x);
	nl();
	}

void def_type_output(void)
	{
	def0("nl", op_nl);
	def1("say", say_str);
	def1("put", put_str);
	}
