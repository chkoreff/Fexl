#include "str.h"

#include "value.h"
#include "fexl.h"
#include "long.h"
#include "qstr.h"

value type_argc(value f)
	{
	return Qlong(argc);
	}

value type_argv(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	long i = get_long(x);
	value z = Qstr0(i >= 0 && i < argc ? argv[i] : "");
	drop(x);
	return z;
	}

/*TODO more functions */
