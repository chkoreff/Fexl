#include "dynamic.h"
#include "value.h"
#include "double.h"
#include "eval.h"
#include "long.h"
#include "resolve.h"
#include "string.h"
#include "sym.h"

/* The standard context for Fexl. */
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

	/* Resolve name "X" by looking up "fexl_X" in the library. */
	void *def = lib_sym("fexl_",name);
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
