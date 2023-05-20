#include <value.h>

#include <ref.h>

value find_pos(unsigned long pos, value cx)
	{
	while (1)
		{
		if (cx->T == &type_ref)
			return 0;
		if (pos == 0)
			return hold(cx->L);
		pos--;
		cx = cx->R;
		}
	}

static value step(value pair)
	{
	return find_pos(pair->L->v_u64, pair->R);
	}

struct type type_ref = { step, no_apply, no_clear };

value R(unsigned long pos)
	{
	value exp = new_exp(&type_ref);
	exp->v_u64 = pos;
	return exp;
	}
