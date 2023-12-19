#include <dlfcn.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_lib.h>
#include <type_str.h>

value type_lib(value fun, value f)
	{
	return type_void(fun,f);
	}

static void clear_lib(value f)
	{
	dlclose(f->v_ptr);
	}

value Qlib(void *lib)
	{
	static struct value clear = {{.N=0}, {.clear=clear_lib}};
	return V(type_lib,&clear,(value)lib);
	}

// (dlopen name) Open the named library, or return void if not successful.
value type_dlopen(value fun, value f)
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
	(void)fun;
	return f;
	}

value type_dlerror(value fun, value f)
	{
	char *msg = dlerror();
	(void)fun;
	(void)f;
	return Qstr0(msg ? msg : "");
	}

// Work around for the -pedantic option in lib_build, avoiding the error:
// ISO C forbids initialization between function pointer and ‘void *’
static type get_type(void *lib, const char *name)
	{
	union { type t; void *p; } u;
	u.p = dlsym(lib,name);
	return u.t;
	}

value type_dlsym(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_lib && y->T == type_str)
		{
		void *lib = x->v_ptr;
		const char *name = str_data(y);
		type t = get_type(lib,name);

		if (t == 0)
			f = hold(Qvoid);
		else
			f = yield(Q(t));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}
