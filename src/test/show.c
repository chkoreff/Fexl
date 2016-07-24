#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_resolve.h>
#include <type_str.h>
#include <type_sym.h>

void show_type(type t)
	{
	if (t == type_A) put("A");
	else if (t == type_I) put_ch('I');
	else if (t == type_T) put_ch('T');
	else if (t == type_F) put_ch('F');
	else if (t == type_Y) put_ch('@');
	else if (t == type_subst) put("subst");
	else if (t == type_void) put("void");
	else if (t == type_cons) put("cons");
	else if (t == type_null) put("null");
	else if (t == type_str) put("str");
	else if (t == type_sym) put("sym");
	else if (t == type_say) put("say");
	else if (t == type_put) put("put");
	else if (t == type_nl) put("nl");
	else if (t == type_resolve) put("resolve");
	else
		{
		put_ch('T');
		put_ulong((unsigned long)t);
		}
	}

void show(value f)
	{
	put("[");
	put_ulong(f->N);put_ch(' ');
	show_type(f->T);
	if (f->L)
		{
		put_ch(' ');
		show(f->L);
		put_ch(' ');
		show(f->R);
		}
	else if (f->R)
		{
		put_ch(' ');
		if (f->T == type_num)
			put_num(data(f));
		else if (f->T == type_str)
			{
			put_ch('"');
			put_str(data(f));
			put_ch('"');
			}
		else
			put("?");
		}
	put("]");
	}
