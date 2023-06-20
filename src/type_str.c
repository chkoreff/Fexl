#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>

#include <type_str.h>

static void clear_str(value f)
	{
	str_free(f->v_ptr);
	}

value type_str(value fun, value arg)
	{
	return type_void(fun,arg);
	}

const char *str_data(value x)
	{
	return ((string)x->v_ptr)->data;
	}

value Qstr(string x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_str}};
	return V(type_str,&clear,(value)x);
	}

value Qstr0(const char *data)
	{
	return Qstr(str_new_data0(data));
	}

// (. x y) is the concatenation of strings x and y.
value type_concat(value fun, value arg)
	{
	if (fun->L == 0)
		return need(fun,arg,type_str);
	else
		{
		arg = eval(arg);
		if (arg->T == type_str)
			{
			value g = Qstr(str_concat(fun->R->v_ptr, arg->v_ptr));
			drop(fun);
			drop(arg);
			return g;
			}
		else
			return type_void(fun,arg);
		}
	}
