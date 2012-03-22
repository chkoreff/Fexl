#include <dlfcn.h>
#include "die.h"
#include "dynamic.h"

void *open_lib(const char *name)
	{
	void *lib = dlopen(name, RTLD_LAZY);
	if (!lib) die("open_lib: %s", dlerror());

	return lib;
	}

/*
Get the library definition for a symbol.  If you use lib == 0, it searches the
libraries linked with the executable.
*/
void *lib_sym(void *lib, const char *sym)
	{
	void *def = dlsym(lib, sym);
	char *err = dlerror();
	if (err) die("lib_sym: %s", err);

	return def;
	}

void close_lib(void *lib)
	{
	dlclose(lib);
	}
