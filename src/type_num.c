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
