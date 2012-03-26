#include "dynamic.h"
#include "value.h"
#include "long.h"
#include "stack.h"
#include "string.h"

value type_type(value f) { return f; }

/* Make a new value of type "type".  */
value Qtype(type T)
	{
	value f = Qlong((long)T);
	f->T = type_type;
	return f;
	}

type get_type(value f)
	{
	return (type)f->R->L;
	}

value fexl_type_of(value f)
	{
	return Qtype(f->R->T);
	}

/* type_eq x y yes no */
value fexl_type_eq(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return f;

	value x = f->L->L->L->R;
	value y = f->L->L->R;

	if (!arg(type_type,x)) return f;
	if (!arg(type_type,y)) return f;

	if (get_type(x) == get_type(y)) return f->L->R;
	return f->R;
	}

/* atomic x yes no */
value fexl_atomic(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;

	if (!x->L) return f->L->R;
	return f->R;
	}

/* (type_named name) returns the type with the given name, or type 0 if there
is no such type. */
void *fexl_type_named(value f)
	{
	value x = f->R;
	if (!arg(type_string,x)) return f;
	return Qtype(lib_sym("type_",string_data(x)));
	}
