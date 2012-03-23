#include <dlfcn.h> /*TODO*/
#include <stdio.h> /*TODO*/
#include <string.h>  /* strcmp */
#include "buf.h"
#include "die.h"
#include "value.h"
#include "double.h"
#include "io.h" /*TODO*/
#include "long.h"
#include "memory.h"
#include "resolve.h"
#include "stack.h"
#include "string.h"

/* The standard context for Fexl. */

/*TODO perhaps also a same_type function for checking types at run-time */
/*TODO pair */
/*TODO append */
/*TODO + - * / = < etc. */

/*TODO use lib_sym(0,name) -- except that has a built-in die */

/* Look up type_[name] in the Fexl library linked with the executable. */
struct value *resolve_name(char *name)
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

struct value *resolve(struct value *sym)
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
	struct value *f = resolve_name(canonical_name(name));
	if (f) return f;

	/*TODO report line numbers */
	/*TODO report all the undefined symbols, not just first.  We'll do that
	with the chained resolution trick so it all happens inside Fexl. */
	fputs("Undefined symbol ", stderr);
	/*TODO unify with quote_string_put */
	char *quote = sym->T == type_string ? "\"" : "";
	fputs(quote, stderr);
	fwrite(string_data(sym), 1, string_len(sym), stderr);
	fputs(quote, stderr);
	nl();
	die("TODO");

	return sym; /*TODO*/
	}

/*TODO how do we distinguish name vs. string in a resolution function written
in Fexl? */
struct value *type_resolve(struct value *f)
	{
	if (!f->L->L) return f;

	struct value *sym = f->L->R;
	struct value *body = f->R;

	if (!sym->T) { push(sym); return f; } /*TODO test*/

	#if 0
	print("resolve sym  = ");show(sym);nl();
	#endif
	return A(body,resolve(sym));
	}
