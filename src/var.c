#include "value.h"
#include "basic.h"

value type_var(value f)
	{
	if (!f->N) drop(f->R);
	return 0;
	}

/* var creates a new variable with initial value C. */
value fexl_var(value f)
	{
	value x = C;
	hold(x);
	return V(type_var,0,x);
	}

/* (set var val) Set the variable to the new value. */
value fexl_set(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value var = eval(f->L->R);
	data_type(var,type_var);

	value val = eval(f->R);
	drop(var->R);
	var->R = val;

	drop(var);
	return I;
	}

/* (get var) Return the current value of the variable. */
value fexl_get(value f)
	{
	if (!f->L) return 0;
	value var = eval(f->R);
	data_type(var,type_var);

	value val = var->R;

	drop(var);
	return val;
	}
