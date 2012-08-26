#include "dynamic.h"
#include "value.h"
#include "basic.h"
#include "double.h"
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

	/* Resolve name "X" by looking up "reduce_X" in the library. */
	{
	void *def = lib_sym("reduce_",name);
	if (def)
		return Q(def);
	}

	/* Resolve name "X" by looking up "const_X" in the library and evaluating
	that function to get the constant value. */
	{
	void *def = lib_sym("const_",name);
	if (def)
		{
		value (*f)(void) = def;
		return f();
		}
	}

	return 0;
	}

/*
Resolve a symbol in the standard Fexl context.
The value of (resolve sym) is
  yes def      # if the symbol is defined as def
  no           # if the symbol is not defined
*/
void reduce_resolve(value f)
	{
	value sym = f->R;
	eval(sym);

	void *def = resolve(sym);
	if (def)
		replace_apply(f, Q(reduce_yes), def);
	else
		replace(f, Q(reduce_F));
	}
