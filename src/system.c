#include "str.h"

#include "value.h"
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

/*LATER more functions
set_rlimit
*/
