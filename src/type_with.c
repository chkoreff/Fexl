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
		return (str_cmp(get_str(x),get_str(y)) == 0);
	else if (x->T == type_num && y->T == type_num)
		return get_double(x) == get_double(y);
	else
		return 0;
	}

value type_assoc(value f)
	{
	if (f->N == 0)
		{
		drop((value)f->R->T);
		drop(f->R);
		return 0;
		}

	if (!f->L) return 0;
	{
	value x = arg(f->R);
	value curr = f->L;
	while (1)
		{
		value key = (value)curr->R->T;
		if (cmp_key(x,key))
			{
			f = hold(curr->R->L);
			break;
			}
		curr = curr->R->R;
		if (curr->T != type_assoc)
			{
			f = A(hold(curr),hold(x));
			break;
			}
		}
	drop(x);
	return f;
	}
	}

static value Qassoc(value key, value val, value obj)
	{
	return D(type_assoc,V((type)key,val,obj));
	}

/* (with key val obj)
Return an object like obj but with key defined as val.
Equivalent to:
\with=(\key\val\obj \obj=obj \x eq x key val; obj x)
*/
value type_with(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value key = arg(f->L->L->R);
	value val = hold(f->L->R);
	value obj = arg(f->R);
	return Qassoc(key,val,obj);
	}
	}

/* (fetch v k x)
Return the value at key k in index v.  If no value, store the value of x in
the index so you get the same value next time.
Equivalent to:
\fetch=
	(\v\k\x
	\y=(var_get v k)
	is_defined y y;
	\x=x
	var_put v (with k x; var_get v)
	x
	)
*/
value type_fetch(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value v = arg(f->L->L->R);
	value k = arg(f->L->R);
	value x = hold(f->R);
	if (v->T == type_var)
		{
		value y = eval(A(hold(v->R),hold(k)));
		if (y->T != type_void)
			f = y;
		else
			{
			drop(y);
			x = eval(x);
			y = Qassoc(hold(k),hold(x),hold(v->R));
			drop(v->R);
			v->R = y;
			f = hold(x);
			}
		}
	else
		f = hold(&Qvoid);
	drop(v);
	drop(k);
	drop(x);
	return f;
	}
	}
