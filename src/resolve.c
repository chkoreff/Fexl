#include <dlfcn.h>
#include <string.h>  /* strcmp */
#include "buf.h"
#include "die.h"
#include "value.h"
#include "double.h"
#include "long.h"
#include "memory.h"
#include "resolve.h"
#include "run.h"
#include "stack.h"
#include "string.h"

/* The standard context for Fexl. */

/*TODO append */
/*TODO + - * / = < etc. */

/* Look up type_[name] in the Fexl library linked with the executable. */
value resolve_name(char *name)
	{
	struct buf *buf = 0;
	buf_add_string(&buf, "type_");
	buf_add_string(&buf, name);
	long len;
	char *string = buf_clear(&buf,&len);

	void *def = dlsym(0, string);
	char *err = dlerror();

	free_memory(string, len+1);
	if (err) return 0;
	return Q(def);
	}

/* Map synonyms to canonical names. */
char *canonical_name(char *name)
	{
	if (strcmp(name,"?") == 0) return "query";
	if (strcmp(name,"T") == 0) return "C";
	if (strcmp(name,"end") == 0) return "C";
	if (strcmp(name,"long_type") == 0) return "is_long";
	if (strcmp(name,"double_type") == 0) return "is_double";
	if (strcmp(name,"string_type") == 0) return "is_string";
	return name;
	}

value resolve(value sym, long line_no)
	{
	if (sym->T == type_string) return sym;

	char *name = string_data(sym);

	/* Integer number (long) */
	{
	long num;
	if (string_long(name,&num)) return Qlong(num);
	}

	/* Floating point number (double) */
	{
	double num;
	if (string_double(name,&num)) return Qdouble(num);
	}

	/* Resolve the canonical name. */
	value f = resolve_name(canonical_name(name));
	if (f) return f;

	/*TODO report all the undefined symbols, not just first.  We'll do that
	with the chained resolution trick so it all happens inside Fexl. */
	line = line_no;
	die("Undefined symbol %s",string_data(sym));

	return sym;
	}

/* resolve sym place exp */
value type_resolve(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value sym = f->L->L->R;
	value place = f->L->R;
	value exp = f->R;

	/*TODO can't use arg here yet because the type is either name or string */
	if (!sym->T) { push(sym); return f; } /*TODO test*/
	if (!arg(type_long,place)) return f; /*TODO test */

	return A(exp,resolve(sym,get_long(place)));
	}
