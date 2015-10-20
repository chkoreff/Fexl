#include <value.h>
#include <basic.h>
#include <input.h>
#include <parse.h>
#include <str.h>
#include <type_input.h>
#include <type_parse_string.h>
#include <type_resolve.h>
#include <type_str.h>

static value parse_string(string text)
	{
	input save_getd = getd;
	string save_cur_text = cur_text;
	unsigned long save_cur_pos = cur_pos;

	getd = getd_string;
	cur_text = text;
	cur_pos = 0;

	{
	value exp = parse_source("");

	getd = save_getd;
	cur_text = save_cur_text;
	cur_pos = save_cur_pos;
	return exp;
	}
	}

/* (parse_string str context) = {f}, where f is the function specified by the
string in the given context. */
value type_parse_string(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_str)
		{
		value exp = parse_string(data(x));
		return single(
			A(A(A(Q(type_resolve), Qstr(str_new_data0(""))), exp), hold(f->R))
			);
		}
	reduce_void(f);
	return 0;
	}
	}
