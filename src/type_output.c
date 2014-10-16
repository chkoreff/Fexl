#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

/*LATER allow list value */
value type_put(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		put_str((string)x->R);
	else if (x->T == type_num)
		put_num((number)x->R);
	else if (x->T == type_T)
		put_ch('T');
	else if (x->T == type_F)
		put_ch('F');

	f = hold(I);
	drop(x);
	return f;
	}
	}

value type_nl(value f)
	{
	nl();
	f = hold(I);
	return f;
	}

value type_say(value f)
	{
	f = type_put(f);
	if (f == I) nl();
	return f;
	}

value type_put_to_error(value f)
	{
	if (!f->L) return f;
	{
	output save_putd = putd;
	put_to_error();
	f = eval(hold(f->R));
	putd = save_putd;
	return f;
	}
	}
