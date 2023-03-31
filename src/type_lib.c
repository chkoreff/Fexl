#include <dlfcn.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_lib.h>
#include <type_str.h>

value type_lib(value f)
	{
	return type_atom(f);
	}

static void clear_lib(value f)
	{
	dlclose(f->v_ptr);
	}

value Qlib(void *lib)
	{
	static struct value atom = {{.N=0}, {.clear=clear_lib}};
	return V(type_lib,&atom,(value)lib);
	}

/* (dlopen name) Open the named library, or return void if not successful.  */
value type_dlopen(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *name = str_data(x);
		void *lib = dlopen(name,RTLD_NOW|RTLD_NODELETE);
		if (lib == 0)
			f = hold(Qvoid);
		else
			f = Qlib(lib);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_dlerror(value f)
	{
	char *msg = dlerror();
	(void)f;
	return Qstr0(msg ? msg : "");
	}

value type_dlsym(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_lib && y->T == type_str)
		{
		void *lib = x->v_ptr;
		const char *name = str_data(y);
		type t = dlsym(lib,name);

		if (t == 0)
			f = hold(Qvoid);
		else
			f = AV(hold(Qyield),V(t,0,0));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}
