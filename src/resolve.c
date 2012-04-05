#include <string.h>  /* strcmp */

#include "buf.h"
#include "die.h"
#include "dynamic.h"
#include "memory.h"
#include "run.h"

#include "value.h"
#include "basic.h"
#include "double.h"
#include "eval.h"
#include "long.h"
#include "resolve.h"
#include "string.h"
#include "sym.h"

/* The standard context for Fexl. */

/* LATER + - * / = < etc. */
/* LATER file operations */

/* Map synonyms to canonical names. */
/* LATER move out to fexl.fxl */
static char *canonical_name(char *name)
	{
	if (strcmp(name,"?") == 0) return "query";
	if (strcmp(name,"T") == 0) return "C";
	if (strcmp(name,"end") == 0) return "C";
	if (strcmp(name,"long_type") == 0) return "is_long";
	if (strcmp(name,"double_type") == 0) return "is_double";
	if (strcmp(name,"string_type") == 0) return "is_string";
	return name;
	}

value resolve(value sym)
	{
	if (sym->T == type_string) return sym;
	if (sym->T != type_name) return 0;

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
	void *def = lib_sym("fexl_",canonical_name(name));
	if (def) return Q(def);

	return 0;
	}

/*
Resolve a symbol in the standard Fexl context.
The value of (resolve sym yes no) is
  yes def      # if the symbol is defined as def
  no           # if the symbol is not defined
*/
value fexl_resolve(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value sym = f->L->L->R;
	if (sym->T == 0) eval(sym);

	void *def = resolve(sym);
	if (def) return A(f->L->R,def);
	return f->R;
	}
