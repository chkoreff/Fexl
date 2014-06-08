#include <value.h>

#include <num.h>
#include <stdio.h> /* snprintf */
#include <str.h>
#include <sys_stdio.h>
#include <type_convert.h>
#include <type_num.h>
#include <type_str.h>

/*TODO any way to unify with put_double? */
string num_str(number x)
	{
	char buf[100]; /* Being careful here. */
	snprintf(buf, sizeof(buf), "%.15g", *x);
	return str_new_data0(buf);
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
		n = num_new_double(0); /*TODO return undef */

	y = Qnum(n);
	drop(x);
	return y;
	}
