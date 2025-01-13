#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <dlfcn.h>
#include <type_load.h>
#include <type_str.h>

value type_load_so(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *lib_name = str_data(x);
		void *lib = dlopen(lib_name,RTLD_NOW|RTLD_NODELETE);
		if (lib == 0)
			die(dlerror());
		else
			{
			const char *sym_name = "run";
			void (*run)(void) = dlsym(lib,sym_name);
			if (run == 0)
				die(dlerror());
			else
				run();
			dlclose(lib);
			}
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
