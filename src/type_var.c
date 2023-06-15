#include <value.h>

#include <basic.h>
#include <context.h>

#include <type_var.h>

// Variable

static void clear_var(value f)
	{
	drop(f->R);
	}

struct type type_var = { 0, apply_void, clear_var };

// Create variable

static value op_var_new(void)
	{
	return Q(&type_var,hold(Qvoid));
	}

// Put value in variable

static value apply_var_put(value f, value x)
	{
	if (f->L == 0)
		return need(f,x,&type_var);
	else
		{
		value v = f->R;
		x = eval(x);
		drop(v->R);
		v->R = x;
		return hold(QI);
		}
	}

static struct type type_var_put = { 0, apply_var_put, clear_T };

// Get value from variable

static value apply_var_get(value f, value x)
	{
	x = eval(x);
	if (x->T == &type_var)
		f = hold(x->R);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static struct type type_var_get = { 0, apply_var_get, no_clear };

void use_var(void)
	{
	define_op("var_new", op_var_new);
	define("var_put", Q(&type_var_put,0));
	define("var_get", Q(&type_var_get,0));
	}
