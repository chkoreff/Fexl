#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static void putv(value x)
	{
	x = eval(hold(x));
	while (1)
		{
		if (x->T == type_str)
			put_str(data(x));
		else if (x->T == type_num)
			put_num(data(x));
		else if (x->T == type_T)
			put_ch('T');
		else if (x->T == type_F)
			put_ch('F');
		else if (x->T == type_cons && x->L && x->L->L)
			{
			putv(x->L->R);
			/* Eliminated tail recursive call putv(x->R) here. */
			{
			value y = eval(hold(x->R));
			drop(x);
			x = y;
			continue;
			}
			}
		drop(x);
		return;
		}
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	putv(f->R);
	return hold(I);
	}

value type_nl(value f)
	{
	(void)f;
	nl();
	return hold(I);
	}

value type_say(value f)
	{
	f = type_put(f);
	if (f == I) nl();
	return f;
	}

/* (put_to_error x) = x.  Evaluates x with all output going to stderr. */
value type_put_to_error(value f)
	{
	if (!f->L) return 0;
	{
	output save = putd;
	put_to_error();
	f = eval(hold(f->R));
	putd = save;
	return f;
	}
	}
