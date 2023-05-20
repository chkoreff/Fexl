#include <value.h>
#include <app.h>
#include <ref.h>

#include <basic.h>
#include <str.h>

#include <type_str.h>

static void clear(value exp)
	{
	str_free(exp->v_ptr);
	}

struct type type_str = { no_step, apply_atom, clear };

// TODO common R(0)

static value op_str2(value pair, string op(string,string))
	{
	value x = pair->R->R->L->L;
	value y = pair->R->L->L;
	if (x->T == &type_str && y->T == &type_str)
		return A(Qstr(op(x->v_ptr,y->v_ptr)),R(0));
	else
		return A(hold(Qvoid),R(0));
	}

// (. x y) is the concatenation of strings x and y.
static value step_concat(value pair)
	{
	return op_str2(pair,str_concat);
	}

struct type type_concat = { step_concat, no_apply, no_clear };

value Qstr(string x)
	{
	value exp = new_exp(&type_str);
	exp->v_ptr = x;
	return exp;
	}
