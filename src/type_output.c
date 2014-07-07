#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_str.h>
#include <type_output.h>

int putv(value x)
	{
	if (x->T == type_str)
		put_str(get_str(x));
	else if (x->T == type_num)
		put_num(get_num(x));
	else if (x->T == type_void)
		;
	else
		return 0;

	return 1;
	}

void type_put(value f)
	{
	if (f->L->L)
		{
		if (putv(eval(f->L->R)))
			replace(f,f->R);
		else
			replace_void(f);
		}
	}

void type_nl(value f)
	{
	nl();
	replace(f,f->R);
	}

void type_say(value f)
	{
	if (f->L->L)
		{
		if (putv(eval(f->L->R)))
			{
			nl();
			replace(f,f->R);
			}
		else
			replace_void(f);
		}
	}
