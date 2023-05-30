#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <output.h>
#include <type_str.h>

#include <type_output.h>

static value apply_print(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_str)
		{
		void (*op)(string) = f->v_ptr;
		op(x->v_ptr);
		}
	drop(x);
	return hold(QI);
	}

static struct type type_print = { 0, apply_print, no_clear };

static void define_print(const char *name, void op(string))
	{
	define(name, Q(&type_print,op));
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
	define_print("say", say_str);
	define_print("put", put_str);
	// LATER fput fsay
	}
