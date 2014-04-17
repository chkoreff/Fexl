#include <value.h>
#include <stdio.h>
#include <basic.h>
#include <fexl.h>
#include <stdlib.h>
#include <system.h>
#include <type_long.h>
#include <type_str.h>

#if 0
value const_fexl_argc(void) { return Qlong(argc); } /*TODO*/
#endif

value type_argv(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long i = get_long(x);
	value z = Qstr0(i >= 0 && i < argc ? argv[i] : "");
	drop(x);
	return z;
	}

/*TODO env*/

value type_exit(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long status = get_long(x);
	drop(x);
	exit(status);
	return I;
	}

/*LATER more functions
set_rlimit
*/
