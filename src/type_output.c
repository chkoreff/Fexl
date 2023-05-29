#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <type_str.h>

#include <type_output.h>

static value apply_str_I(value f, value x)
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

static struct type type_str_I = { 0, apply_str_I, no_clear };

static void define_str_I(const char *name, void op(string))
	{
	define(name, Q(&type_str_I,op));
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

void use_output(void)
	{
	define_op("nl", op_nl);
	define_str_I("say", say_str);
	define_str_I("put", put_str);
	}
