#include <value.h>

#include <basic.h>
#include <type_var.h>

value type_var(value f)
	{
	if (!f->N) drop(f->R);
	return 0;
	}

/* (var_new) creates a new variable with initial value C. */
value type_var_new(value f)
	{
	(void)f;
	return D(type_var,hold(C));
	}

/* (var_put var val) Set the variable to the new value. */
value type_var_put(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value var = arg(f->L->R);
	(void)get_data(var,type_var);

	value val = arg(f->R);
	drop(var->R);
	var->R = val;

	drop(var);
	return hold(I);
	}

/* (var_get var) Return the current value of the variable. */
value type_var_get(value f)
	{
	if (!f->L) return 0;
	value var = arg(f->R);
	value val = get_data(var,type_var);
	drop(var);
	return hold(val);
	}
