#include <dlfcn.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <type_load.h>
#include <type_str.h>
#include <type_sym.h>

value type_load(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *name = str_data(x);
		void *lib = dlopen(name,RTLD_NOW|RTLD_NODELETE);
		if (lib == 0)
			die(dlerror());
		{
		type t = dlsym(lib,"type_context");
		if (t == 0)
			die(dlerror());
		f = Q(t);
		dlclose(lib);
		}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
