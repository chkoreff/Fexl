static value replace_look(value f, const char *type, value data)
	{
	return replace_apply(f, A(f->R,Qstring(type)), data);
	}

#include <stdio.h> /*TODO*/

/* (look x :) = (: type data) */
value type_look(value f)
	{
	if (!f->L || !f->L->L) return 0;

	printf("calling look %ld\n", (long)f);

	value x = eval(f->L->R);
	type t = x->T;

	if (t == type_long)
		return replace_look(f, "long", x);
	else if (t == type_double)
		return 0;
	else if (t == type_string)
		return 0;
	else if (t == type_C)
		return 0;
	else if (t == type_item)
		{
		if (x->L && x->L->L)
			return 0;
		else
			return 0;
		}
	#if 0
	else if (t == type_tag) /*TODO*/
		return 0;
	#endif
	else
		return 0;
	}

value resolve_type(const char *name)
	{
	if (strcmp(name,"look") == 0) return Q(type_look);
	return 0;
	}
