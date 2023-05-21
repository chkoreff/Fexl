#include <stdlib.h> // strtod

#include <value.h>
#include <basic.h>

#include <str.h>
#include <type_str.h>
#include <format.h>

#include <type_num.h>

static void clear(value exp)
	{
	(void)exp;
	}

struct type type_num = { no_step, apply_atom, clear };

value Qnum(double x)
	{
	value exp = new_exp(&type_num);
	exp->v_double = x;
	return exp;
	}

value Qnum_str0(const char *name)
	{
	char *end;
	double val;
	val = strtod(name, &end);
	if (*end == '\0')
		return Qnum(val);
	else
		return 0;
	}

static value step_num_str(value pair)
	{
	value x = pair->R->L->L;
	if (x->T == &type_num)
		return V(Qstr0(format_double(x->v_double)));
	else
		return V(hold(Qvoid));
	}

struct type type_num_str = { step_num_str, no_apply, no_clear };
