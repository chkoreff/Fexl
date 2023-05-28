#include <str.h>
#include <value.h>

#include <basic.h>
#include <context.h>
#include <memory.h>

#include <type_str.h>

static void clear_str(value f)
	{
	str_free(f->v_ptr);
	}

struct type type_str = { 0, apply_void, clear_str };

string get_str(value x)
	{
	return x->v_ptr;
	}

const char *str_data(value x)
	{
	return get_str(x)->data;
	}

value Qstr(string x)
	{
	return Q(&type_str,x);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

static value apply_str_str(value f, value x)
	{
	value g;

	x = eval(x);
	if (f->L == 0)
		{
		if (x->T == &type_str)
			g = V(f->T,hold(f),hold(x));
		else
			g = hold(Qvoid);
		}
	else
		{
		if (x->T == &type_str)
			{
			string (*op)(string,string) = f->L->v_ptr;
			g = Qstr(op(f->R->v_ptr, x->v_ptr));
			}
		else
			g = hold(Qvoid);
		}

	drop(f);
	drop(x);
	return g;
	}

static struct type op_str_str = { 0, apply_str_str, clear_T };

void def_type_str(void)
	{
	// (. x y) is the concatenation of strings x and y.
	define(".", Q(&op_str_str, str_concat));
	// LATER length
	// LATER slice
	// LATER search
	// LATER str_num
	// LATER ord
	// LATER chr
	// LATER char_width
	// LATER dirname
	// LATER basename
	// LATER length_common
	// LATER compare_at
	}
