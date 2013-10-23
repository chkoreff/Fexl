#include "str.h"

#include "value.h"
#include "basic.h"
#include "fexl.h"
#include "long.h"
#include "qstr.h"

value const_fexl_argc(void) { return Qlong(argc); }

value fexl_argv(value f)
	{
	if (!f->L) return 0;
	value x = eval(f->R);
	long i = get_long(x);
	value z = Qstr0(i >= 0 && i < argc ? argv[i] : "");
	drop(x);
	return z;
	}

/* (examine x) returns (pair type content).  The type of x is a long value.
The content of x is [] if x is an atom, or [left;right] if x is the application
of left to right. */
value fexl_examine(value f)
	{
	if (!f->L) return 0;
	value x = eval(f->R);
	value the_type = Qlong((long)x->T);
	value the_content = x->L == 0 ? C : item(x->L,x->R);
	value result = pair(the_type,the_content);
	drop(x);
	return result;
	}

/*LATER more functions
set_rlimit
*/
