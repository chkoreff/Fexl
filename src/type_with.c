#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_num.h>
#include <type_str.h>
#include <type_var.h>
#include <type_with.h>

static int cmp_key(value x, value y)
	{
	if (x->T == type_str && y->T == type_str)
		return str_eq(x->v_ptr,y->v_ptr);
	else if (x->T == type_num && y->T == type_num)
		return x->v_double == y->v_double;
	else
		return 0;
	}

// Look up the value associated with a key.
value type_assoc(value f)
	{
	value obj = f->L;
	value x = arg(f->R);
	while (1)
		{
		value key = obj->L->L;
		if (cmp_key(key,x))
			{
			drop(x);
			return hold(obj->L->R);
			}

		obj = obj->R;
		if (obj->T != type_assoc)
			return A(hold(obj),x);
		}
	}

value Qassoc(value key, value val, value obj)
	{
	return V(type_assoc,pair(key,val),obj);
	}

/*
(with key val obj)
Return a function like obj but with key defined as val.

\with=(\key\\val\obj \x eq x key val; obj x)
*/
value type_with(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value key = arg(f->L->L->R);
	value val = hold(f->L->R);
	value obj = hold(f->R = eval(f->R));
	return Qassoc(key,val,obj);
	}
	}

value type_is_obj(value f)
	{
	return op_is_type(f,type_assoc);
	}

// (split_obj obj next) = (next key val tail), where key and val are the first
// pair in the object, and tail is the value following the first pair.
value type_split_obj(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_assoc)
		{
		value key = hold(x->L->L);
		value val = hold(x->L->R);
		value obj = hold(x->R);
		value next = hold(f->R);
		f = A(A(A(next,key),val),obj);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/*
(fetch v k x)
Return the value at key k in index v.  If no value, store the value of x in
the index so you get the same value next time.

\fetch=
	(\v\k\\x
	\y=(var_get v k)
	is_defined y y;
	\x=x
	var_put v (with k x; var_get v)
	x
	)
*/
value type_fetch(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value v = arg(f->L->L->R);
	if (v->T == type_var)
		{
		value k = arg(f->L->R);
		value y = eval(A(hold(v->R),hold(k)));
		if (y->T != type_void)
			f = y;
		else
			{
			drop(y);
			f = arg(f->R);
			v->R = Qassoc(hold(k),hold(f),v->R);
			}
		drop(k);
		}
	else
		f = hold(Qvoid);
	drop(v);
	return f;
	}
	}
