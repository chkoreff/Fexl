#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sym.h>

void show(value f)
	{
	if (f->L)
		{
		put_ch(f->T == type_sym ? '{' : '(');
		show(f->L);
		put_ch(' ');
		show(f->R);
		put_ch(f->T == type_sym ? '}' : ')');
		}
	else if (f->T == type_num) put_num((number)data(f));
	else if (f->T == type_str)
		{
		put_ch('"');
		put_str((string)data(f));
		put_ch('"');
		}
	else if (f->T == type_sym)
		{
		symbol sym = (symbol)data(f);
		put_ch('{');
		put_str((string)data(sym->name));
		put_ch('}');
		}
	else if (f->T == type_C) put_ch('C');
	else if (f->T == type_I) put_ch('I');
	else if (f->T == type_T) put_ch('T');
	else if (f->T == type_F) put_ch('F');
	else if (f->T == type_Y) put_ch('@');
	else if (f->T == type_subst) put("subst");
	else if (f->T == type_void) put("void");
	else if (f->T == type_cons) put("cons");
	else if (f->T == type_null) put("null");
	else if (f->T == type_add) put("+");
	else if (f->T == type_concat) put_ch('.');
	else if (f->T == type_slice) put("slice");
	else if (f->T == type_length) put("length");
	else if (f->T == type_str_num) put("str_num");
	else if (f->T == type_say) put("say");
	else if (f->T == type_put) put("put");
	else if (f->T == type_nl) put("nl");
	else
		{
		put_ch('T');
		put_ulong((unsigned long)f->T);
		}
	}
