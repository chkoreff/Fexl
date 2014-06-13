#include <num.h>
#include <value.h>

#include <basic.h>
#include <format.h>
#include <str.h>
#include <type_convert.h>
#include <type_num.h>
#include <type_str.h>

string num_str(number x)
	{
	return str_new_data0(format_num(x));
	}

value type_num_str(value f)
	{
	value x, y;
	if (!f->L) return 0;
	x = arg(f->R);
	y = Qstr(num_str(get_num(x)));
	drop(x);
	return y;
	}

value type_str_num(value f)
	{
	value x, y;
	number n;
	if (!f->L) return 0;
	x = arg(f->R);
	n = str_num(get_str(x)->data);
	if (n == 0)
		y = hold(C); /* invalid, return [] */
	else
		y = Qnum(n);

	drop(x);
	return y;
	}
