#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

value type_put(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g = 0;
	if (is_atom(type_str,x))
		{
		put_str((string)x->R);
		g = hold(I);
		}
	else if (is_atom(type_num,x))
		{
		put_num((number)x->R);
		g = hold(I);
		}
	drop(x);
	return g;
	}
	}

value type_nl(value f)
	{
	(void)f;
	nl();
	return hold(I);
	}

value type_say(value f)
	{
	value g = type_put(f);
	if (g == I) nl();
	return g;
	}

value type_put_to_error(value f)
	{
	if (!f->L) return f;
	{
	output save_putd = putd;
	value g;
	put_to_error();
	g = eval(hold(f->R));
	putd = save_putd;
	return g;
	}
	}
