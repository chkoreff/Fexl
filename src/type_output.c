#include <value.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

value type_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value xv = arg(&f->L->R);

	{
	string x = atom(type_str,xv);
	if (x)
		{
		put_str(x);
		return f->R;
		}
	}

	{
	number x = atom(type_num,xv);
	if (x)
		{
		put_num(x);
		return f->R;
		}
	}

	return 0;
	}
	}

value type_nl(value f)
	{
	if (!f->L) return 0;
	nl();
	return f->R;
	}

value type_say(value f)
	{
	value g = type_put(f);
	if (g) nl();
	return g;
	}
