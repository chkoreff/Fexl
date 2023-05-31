#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_limit.h>
#include <type_math.h>
#include <type_meta.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

#include <context.h>

value cx_cur; // Current context

void define(const char *key, value val)
	{
	cx_cur = A(A(Qstr0(key),val),cx_cur);
	}

void define_op(const char *name, value op(void))
	{
	define(name, Q(&type_Z,op));
	}

static value apply_define(value f, value x)
	{
	if (f->L == 0)
		{
		x = eval(x);
		if (x->T == &type_str)
			return V(f->T,hold(f),x);
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	else
		{
		const char *key = str_data(f->R);
		define(key,x);
		return hold(QI);
		}
	}

static struct type type_define = { 0, apply_define, clear_T };

// Define initial context.

void beg_context(void)
	{
	beg_basic();

	cx_cur = hold(QI); // empty stack

	define("define",Q(&type_define,0));
	use_basic();
	use_num();
	use_str();
	use_math();
	use_output();
	use_meta();
	use_limit();

	// LATER time
	// LATER rand
	// LATER crypto
	}

void end_context(void)
	{
	drop(cx_cur);
	end_basic();
	}
