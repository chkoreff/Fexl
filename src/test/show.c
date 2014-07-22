#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <str.h>
#include <test/show.h>
#include <type_cmp.h>
#include <type_convert.h>
#include <type_math.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>
#include <type_sym.h>

void (*show_other)(value f) = 0;

static void do_show(value f)
	{
	if (remain_steps > 0) remain_steps--; else { put_ch('_'); return; }
	if (remain_depth > 0) remain_depth--; else { put_ch('_'); return; }

	if (!f)
		put_ch('?');
	else if (f->L)
		{
		put_ch(f->T == type_sym ? '{' : '(');
		do_show(f->L);
		put_ch(' ');
		do_show(f->R);
		put_ch(f->T == type_sym ? '}' : ')');
		}
	else if (f->T == type_C) put_ch('C');
	else if (f->T == type_S) put_ch('S');
	else if (f->T == type_I) put_ch('I');
	else if (f->T == type_J) put_ch('J');
	else if (f->T == type_F) put_ch('F');
	else if (f->T == type_R) put_ch('R');
	else if (f->T == type_L) put_ch('L');
	else if (f->T == type_Y) put_ch('@');
	else if (f->T == type_str)
		{
		put_ch('"');
		put_str((string)f->R);
		put_ch('"');
		}
	else if (f->T == type_sym)
		{
		struct sym *sym = (struct sym *)f->R;
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
	else if (f->T == type_div) put("/");
	else if (f->T == type_lt) put("lt");
	else if (f->T == type_eq) put("eq");
	else if (f->T == type_ge) put("ge");
	else if (f->T == type_sqrt) put("sqrt");
	else if (f->T == type_num_str) put("num_str");
	else if (f->T == type_str_num) put("str_num");
	else if (f->T == type_put) put("put");
	else if (f->T == type_nl) put("nl");
	else if (f->T == type_say) put("say");
	else if (show_other) show_other(f);
	else put_ch('?');

	remain_depth++;
	}

void show(value f)
	{
	unsigned long save_remain_depth = remain_depth;
	unsigned long save_remain_steps = remain_steps;
	remain_depth = 50;
	remain_steps = 200;
	do_show(f);
	remain_depth = save_remain_depth;
	remain_steps = save_remain_steps;
	}
