#include <dlfcn.h>
#include "str.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "qstr.h"

value type_lib(value f)
	{
	if (!f->N) dlclose((void *)f->R);
	return f;
	}

/*
RTLD_LAZY: Perform lazy binding.  Only resolve symbols as the code that
references them is executed.

RTLD_NODELETE: Do not unload the library during dlclose().  This flag is not
specified in POSIX.1-2001.
NOTE 20131108:  jturner notes that OpenBSD does not support RTLD_NODELETE.  I
could wrap "#ifdef __linux" around it, but I'm just going to remove it for now.
I'm sure I could work around any issues which arise, but we're not even relying
on dlopen at this point anyway.

The dlopen man page says that if the path is NULL, then the returned handle is
for the main program.  That's fine, but when we want to reference the main
program, we pass in an empty string, not a NULL pointer.  The man page doesn't
specify what will happen then.  In practice it seems to work fine, but it might
not be portable.  Therefore I take the extra step of checking for an empty
string and using NULL instead in that case, just to be sure.
*/
value fexl_dlopen(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	const char *path = get_str(x)->data;

	/* Portability guard for unspecified behavior (see above). */
	if (path[0] == '\000') path = 0;

	#if 0
	void *lib = dlopen(path, RTLD_LAZY|RTLD_NODELETE);
	#endif
	void *lib = dlopen(path, RTLD_LAZY);  /* Be compatible with OpenBSD. */

	value result = V(type_lib,0,(value)lib);
	drop(x);
	return result;
	}

value fexl_dlsym(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);

	data_type(x,type_lib);
	void *lib = (void *)x->R;
	void *p = lib ? dlsym(lib, get_str(y)->data) : 0;
	value result = Qlong((long)p);
	drop(x);
	drop(y);
	return result;
	}

value fexl_Q(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	type t = (type)get_long(x);
	value y = A(later,Q(t));
	drop(x);
	return y;
	}
