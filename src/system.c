#include "str.h"

#include "value.h"
#include "basic.h"
#include "fexl.h"
#include "long.h"
#include "qstr.h"

value const_fexl_argc(void) { return Qlong(argc); }

value fexl_argv(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long i = get_long(x);
	value z = Qstr0(i >= 0 && i < argc ? argv[i] : "");
	drop(x);
	return z;
	}

value const_fexl___DATE__(value f) { return Qstr0(__DATE__); }
value const_fexl___TIME__(value f) { return Qstr0(__TIME__); }

/* (like x y) return true if x and y are atoms of the same type. */
value fexl_like(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);
	value result = (x->L == 0 && y->L == 0 && x->T == y->T) ? C : F;
	drop(x);
	drop(y);
	return result;
	}

/*LATER more functions
set_rlimit
*/
