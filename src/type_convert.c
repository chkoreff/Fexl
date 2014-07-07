#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_convert.h>
#include <type_num.h>
#include <type_str.h>

static string num_str(number x)
	{
	return str_new_data0(format_num(x));
	}

void type_num_str(value f)
	{
	number x = get_num(eval(f->R));
	if (x == 0)
		replace_void(f);
	else
		replace_str(f,num_str(x));
	}

void type_str_num(value f)
	{
	string x = get_str(eval(f->R));
	if (x == 0)
		replace_void(f);
	else
		{
		number n = str_num(x->data);
		if (n == 0)
			replace_void(f);
		else
			replace_num(f,n);
		}
	}
