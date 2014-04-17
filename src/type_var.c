#include <value.h>
#include <basic.h>
#include <type_var.h>

value type_var(value f)
	{
	if (!f->N) drop(f->R);
	return f;
	}

/* (new) creates a new variable with initial value C. */
value type_new(value f)
	{
	(void)f;
	value x = C;
	hold(x);
	return V(type_var,0,x);
	}

/* (set var val) Set the variable to the new value. */
value type_set(value f)
	{
	if (!f->L || !f->L->L) return f;

	value var = eval(f->L->R);
	(void)get_data(var,type_var);

	value val = eval(f->R);
	drop(var->R);
	var->R = val;

	drop(var);
	return I;
	}

/* (get var) Return the current value of the variable. */
value type_get(value f)
	{
	if (!f->L) return f;
	value var = eval(f->R);
	value val = get_data(var,type_var);
	drop(var);
	return val;
	}
