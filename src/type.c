#include "dynamic.h"
#include "value.h"
#include "eval.h"
#include "long.h"
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

/*
is_apply x yes no
	= (yes L R)  # if x is the function application (L R)
	= no         # if x is atomic
*/
value fexl_is_apply(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;

	if (x->L) return A(A(f->L->R,x->L),x->R);
	return f->R;
	}

/*
Find the type with the given name.
The value of (type_named name yes no) is
  yes type     # if the type exists
  no           # if not
*/
value fexl_type_named(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	if (!arg(type_string,x)) return f;

	void *def = lib_sym("type_",string_data(x));
	if (def) return A(f->L->R,Qtype(def));
	return f->R;
	}
