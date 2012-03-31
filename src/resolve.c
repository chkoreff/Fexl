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

/* The standard context for Fexl. */

/* TODO + - * / = < etc. */
/* TODO file operations */

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

value resolve(value sym)
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
	void *def = lib_sym("fexl_",canonical_name(name));
	if (def) return Q(def);

	return 0;
	}

/* resolve sym place exp */
value fexl_resolve(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value sym = f->L->L->R;
	value place = f->L->R;
	value exp = f->R;

	if (sym->T == 0) { arg(0,sym); return f; } /*TODO test*/
	if (!arg(type_long,place)) return f; /*TODO test */

	void *def = resolve(sym);
	if (def) return A(exp,def);

	/* Undefined symbol:  warn and return (\yes\no exp sym no no). */

	line = get_long(place);
	warn("Undefined symbol %s",string_data(sym));
	main_exit = 1;

	value C = Q(fexl_C);
	value S = Q(fexl_S);
	value I = Q(fexl_I);
	return A(C,A(A(S,A(exp,sym)),I));
	}
