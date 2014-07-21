#include <value.h>
#include <num.h>
#include <str.h>
#include <type_convert.h>
#include <type_num.h>
#include <type_str.h>

static string num_str(number x)
	{
	return str_new_data0(format_num(x));
	}

value type_num_str(value f)
	{
	if (!f->L) return 0;
	{
	number x = atom(type_num,arg(&f->R));
	if (!x) return 0;
	return Qstr(num_str(x));
	}
	}

value type_str_num(value f)
	{
	if (!f->L) return 0;
	{
	string x = atom(type_str,arg(&f->R));
	if (!x) return 0;
	return Qnum(str_num(x->data));
	}
	}
