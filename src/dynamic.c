#include <dlfcn.h>
#include "buf.h"
#include "dynamic.h"
#include "memory.h"

/* Look up "[prefix]_[name]" in the Fexl library linked with the executable. */
void *lib_sym(char *prefix, char *name)
	{
	struct buf *buf = 0;
	buf_add_string(&buf, prefix);
	buf_add_string(&buf, name);
	long len;
	char *string = buf_clear(&buf,&len);

	void *def = dlsym(0, string);

	free_memory(string, len+1);
	return def;
	}
