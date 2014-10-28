#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static void putv(value f)
	{
	f = eval(hold(f));
	while (1)
		{
		if (f->T == type_str)
			put_str((string)f->R);
		else if (f->T == type_num)
			put_num((number)f->R);
		else if (f->T == type_T)
			put_ch('T');
		else if (f->T == type_F)
			put_ch('F');
		else if (f->T == type_cons && f->L && f->L->L)
			{
			putv(f->L->R);
			/* Removed the tail recursive call putv(f->R) here, enabling long
			or infinite lists to print in constant space. */
			{
			value g = eval(hold(f->R));
			drop(f);
			f = g;
			continue;
			}

			}

		drop(f);
		break;
		}
	}

value type_put(value f)
	{
	if (!f->L) return f;
	putv(f->R);
	f = hold(I);
	return f;
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
	output save = putd;
	put_to_error();
	f = eval(hold(f->R));
	putd = save;
	return f;
	}
	}
