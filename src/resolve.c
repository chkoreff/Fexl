#include <stdio.h> /*TODO*/
#include <string.h>  /* strcmp */
#include "value.h"
#include "basic.h"
#include "die.h"
#include "double.h"
#include "io.h"
#include "list.h"
#include "long.h"
#include "meta.h"
#include "parse_string.h"
#include "resolve.h"
#include "show.h" /*TODO*/
#include "stack.h"
#include "string.h"

/* The standard context for Fexl. */

/*TODO perhaps also a same_type function for checking types at run-time */

struct value *resolve(struct value *sym)
	{
	char *name = string_data(sym);

	if (sym->T == type_string) return sym;

	if (strcmp(name, "C") == 0) return C;
	if (strcmp(name, "S") == 0) return S;
	if (strcmp(name, "I") == 0) return I;
	if (strcmp(name, "R") == 0) return R;
	if (strcmp(name, "L") == 0) return L;
	if (strcmp(name, "Y") == 0) return Y;
	if (strcmp(name, "?") == 0) return query;
	if (strcmp(name, "T") == 0) return C;
	if (strcmp(name, "F") == 0) return F;

	if (strcmp(name, "item") == 0) return Q(type_item);
	/* TODO end pair */

	if (strcmp(name, "long_add") == 0) return Q(type_long_add);
	if (strcmp(name, "long_sub") == 0) return Q(type_long_sub);
	if (strcmp(name, "long_mul") == 0) return Q(type_long_mul);
	if (strcmp(name, "long_div") == 0) return Q(type_long_div);
	if (strcmp(name, "long_double") == 0) return Q(type_long_double);
	if (strcmp(name, "is_long") == 0) return Q(type_is_long);
	if (strcmp(name, "long_type") == 0) return Q(type_is_long);
	if (strcmp(name, "long_string") == 0) return Q(type_long_string);
	if (strcmp(name, "long_compare") == 0) return Q(type_long_compare);
	if (strcmp(name, "long_char") == 0) return Q(type_long_char);

	if (strcmp(name, "double_add") == 0) return Q(type_double_add);
	if (strcmp(name, "double_sub") == 0) return Q(type_double_sub);
	if (strcmp(name, "double_mul") == 0) return Q(type_double_mul);
	if (strcmp(name, "double_div") == 0) return Q(type_double_div);
	if (strcmp(name, "is_double") == 0) return Q(type_is_double);
	if (strcmp(name, "double_type") == 0) return Q(type_is_double);
	if (strcmp(name, "double_string") == 0) return Q(type_double_string);
	if (strcmp(name, "double_compare") == 0) return Q(type_double_compare);
	if (strcmp(name, "double_long") == 0) return Q(type_double_long);

	if (strcmp(name, "nl") == 0) return Q(type_nl);
	if (strcmp(name, "string_put") == 0) return Q(type_string_put);
	if (strcmp(name, "is_string") == 0) return Q(type_is_string);
	if (strcmp(name, "string_type") == 0) return Q(type_is_string);
	if (strcmp(name, "string_compare") == 0) return Q(type_string_compare);
	if (strcmp(name, "string_slice") == 0) return Q(type_string_slice);
	if (strcmp(name, "string_append") == 0) return Q(type_string_append);
	if (strcmp(name, "string_len") == 0) return Q(type_string_len);
	if (strcmp(name, "string_long") == 0) return Q(type_string_long);
	if (strcmp(name, "string_double") == 0) return Q(type_string_double);
	if (strcmp(name, "string_at") == 0) return Q(type_string_at);
	if (strcmp(name, "string_common") == 0) return Q(type_string_common);

	if (strcmp(name, "char_get") == 0) return Q(type_char_get);
	if (strcmp(name, "char_put") == 0) return Q(type_char_put);

	if (strcmp(name, "put_max_steps") == 0) return Q(type_put_max_steps);
	if (strcmp(name, "get_max_steps") == 0) return Q(type_get_max_steps);
	if (strcmp(name, "put_max_bytes") == 0) return Q(type_put_max_bytes);
	if (strcmp(name, "get_max_bytes") == 0) return Q(type_get_max_bytes);
	if (strcmp(name, "show") == 0) return Q(type_show);

	if (strcmp(name, "parse") == 0) return Q(type_parse);

	/*TODO + - * / = < etc.  The "singleton" issue will be handled naturally
	by abstracting out the free variables one by one after the parse, rather
	than resolving them inline as I do now. */

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

	/*TODO report line numbers */
	/*TODO report all the undefined symbols, not just first.  We'll do that
	with the chained resolution trick so it all happens inside Fexl. */
	fputs("Undefined symbol ", stderr);
	/*TODO unify with show_string */
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
	print("resolve ");show(f);nl();
	print("  sym  = ");show(sym);nl();
	print("  body = ");show(body);nl();
	#endif
	return A(body,resolve(sym));
	}

/*TODO static combinator.  Also, we can pass the resolution function into
parse.c? */
struct value *Qresolve(struct value *x, struct value *f)
	{
	return A(A(Q(type_resolve),x),lambda(x,f));
	}
