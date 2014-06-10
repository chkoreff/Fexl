#include <str.h>
#include <value.h>

#include <basic.h>
#include <input.h>
#include <num.h>
#include <output.h>
#include <type_file.h>
#include <type_num.h>
#include <type_str.h>
#include <type_var.h>

/* I removed the tail recursion in the cases for type_cons and type_var. */
static void putv(value f)
	{
	while (1)
		{
		if (f->T == type_str)
			{
			string str = get_str(f);
			putd(str->data, str->len);
			}
		else if (f->T == type_num)
			put_num(get_num(f));
		else if (f->T == type_cons)
			{
			value g;
			if (!f->L || !f->L->L || f->L->L->L) bad_type();
			putv(arg(f->L->R));
			g = arg(f->R);
			drop(f);
			f = g;
			continue;
			}
		else if (f->T == type_C)
			{
			if (f->L) bad_type();
			}
		else if (f->T == type_var)
			{
			value g;
			if (f->L) bad_type();
			g = arg(f->R);
			drop(f);
			f = g;
			continue;
			}
		else
			bad_type();

		drop(f);
		break;
		}
	}

value type_get(value f)
	{
	(void)f;
	return Qstr(getd());
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	putv(arg(f->R));
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
	if (!f->L) return 0;
	putv(arg(f->R)); nl();
	return hold(I);
	}
