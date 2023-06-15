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

static value apply_str_str_str(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_str);
	else
		{
		x = eval(x);
		if (x->T == &type_str)
			{
			string (*op)(string,string) = f->L->v_ptr;
			f = Qstr(op(f->R->v_ptr, x->v_ptr));
			drop(x);
			return f;
			}
		else
			{
			drop(x);
			return hold(Qvoid);
			}
		}
	}

static struct type type_str_str_str = { 0, apply_str_str_str, clear_T };

static void define_str_str_str(const char *name, string op(string,string))
	{
	define(name, Q(&type_str_str_str, op));
	}

void use_str(void)
	{
	// (. x y) is the concatenation of strings x and y.
	define_str_str_str(".",str_concat);

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
