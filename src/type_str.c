#include <value.h>

#include <basic.h>
#include <str.h>

#include <type_str.h>

static void clear(value exp)
	{
	str_free(exp->v_ptr);
	}

struct type type_str = { no_step, apply_atom, clear };

value Qstr(string x)
	{
	value exp = new_exp(&type_str);
	exp->v_ptr = x;
	return exp;
	}
