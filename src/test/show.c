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
		put_ch(1,f->T == type_sym ? '{' : '(');
		show(f->L);
		put_ch(1,' ');
		show(f->R);
		put_ch(1,f->T == type_sym ? '}' : ')');
		}
	else if (f->T == type_num) put_num(1,data(f));
	else if (f->T == type_str)
		{
		put_ch(1,'"');
		put_str(1,data(f));
		put_ch(1,'"');
		}
	else if (f->T == type_sym)
		{
		symbol sym = data(f);
		put_ch(1,'{');
		put_str(1,data(sym->name));
		put_ch(1,'}');
		}
	else if (f->T == type_C) put_ch(1,'C');
	else if (f->T == type_I) put_ch(1,'I');
	else if (f->T == type_T) put_ch(1,'T');
	else if (f->T == type_F) put_ch(1,'F');
	else if (f->T == type_Y) put_ch(1,'@');
	else if (f->T == type_subst) put(1,"subst");
	else if (f->T == type_void) put(1,"void");
	else if (f->T == type_cons) put(1,"cons");
	else if (f->T == type_null) put(1,"null");
	else if (f->T == type_add) put(1,"+");
	else if (f->T == type_concat) put_ch(1,'.');
	else if (f->T == type_slice) put(1,"slice");
	else if (f->T == type_length) put(1,"length");
	else if (f->T == type_str_num) put(1,"str_num");
	else if (f->T == type_say) put(1,"say");
	else if (f->T == type_put) put(1,"put");
	else if (f->T == type_nl) put(1,"nl");
	else
		{
		put_ch(1,'T');
		put_ulong(1,(unsigned long)f->T);
		}
	}
