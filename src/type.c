#include "dynamic.h"
#include "value.h"
#include "basic.h"
#include "long.h"
#include "string.h"

void type_type(value f) { type_error(); }

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

void reduce_type_of(value f)
	{
	replace(f, Qtype((type)f->R->T));
	}

/* (type_eq x y) is true if types x and y are equal. */
static void reduce2_type_eq(value f)
	{
	type x = get_type(arg(type_type,f->L->R));
	type y = get_type(arg(type_type,f->R));
	replace_boolean(f, x == y);
	}

void reduce_type_eq(value f)
	{
	f->T = reduce2_type_eq;
	}

/* (atomic x) is true if x is an atomic form. */
void reduce_atomic(value f)
	{
	replace_boolean(f, !f->R->L);
	}

/*
is_apply x yes no
	= (yes L R)  # if x is the function application (L R)
	= no         # if x is atomic
*/
static void reduce3_is_apply(value f)
	{
	value x = f->L->L->R;

	if (x->L)
		replace_apply(f, A(f->L->R,x->L), x->R);
	else
		replace(f, f->R);
	}

static void reduce2_is_apply(value f)
	{
	f->T = reduce3_is_apply;
	}

void reduce_is_apply(value f)
	{
	f->T = reduce2_is_apply;
	}

/*
Find the type with the given name.
The value of (type_named name) is:
  yes type     # if the type exists
  no           # if not
*/
void reduce_type_named(value f)
	{
	value x = arg(type_string,f->R);

	void *def = lib_sym("type_",string_data(x));
	if (def)
		replace_apply(f, Q(reduce_yes), Qtype(def));
	else
		replace(f, Q(reduce_F));
	}
