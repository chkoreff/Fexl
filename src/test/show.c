#include <str.h>
#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <test/show.h>
#include <type_cmp.h>
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
	else if (f->T == type_C) put_ch('C');
	else if (f->T == type_S) put_ch('S');
	else if (f->T == type_I) put_ch('I');
	else if (f->T == type_T) put_ch('T');
	else if (f->T == type_F) put_ch('F');
	else if (f->T == type_R) put_ch('R');
	else if (f->T == type_L) put_ch('L');
	else if (f->T == type_Y) put_ch('@');
	else if (f->T == type_eval) put_ch('!');
	else if (f->T == type_later) put("later");
	else if (f->T == type_cons) put("cons");
	else if (f->T == type_str)
		{
		put_ch('"');
		put_str((string)f->R);
		put_ch('"');
		}
	else if (f->T == type_sym)
		{
		symbol sym = (symbol)f->R;
		put_ch('{');
		if (sym->quoted) put_ch('"');
		put_str((string)sym->name->R);
		if (sym->quoted) put_ch('"');
		put_ch('}');
		}
	else if (f->T == type_num) put_num((number)f->R);
	else if (f->T == type_concat) put("concat");
	else if (f->T == type_length) put("length");
	else if (f->T == type_slice) put("slice");
	else if (f->T == type_add) put("+");
	else if (f->T == type_mul) put("*");
	else if (f->T == type_div) put("/");
	else if (f->T == type_lt) put("lt");
	else if (f->T == type_eq) put("eq");
	else if (f->T == type_ge) put("ge");
	else if (f->T == type_sqrt) put("sqrt");
	else if (f->T == type_num_str) put("num_str");
	else if (f->T == type_str_num) put("str_num");
	else if (f->T == type_is_str) put("is_str");
	else if (f->T == type_is_num) put("is_num");
	else if (f->T == type_put) put("put");
	else if (f->T == type_nl) put("nl");
	else if (f->T == type_say) put("say");
	else put_ch('_');
	}
