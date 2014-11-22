#include <value.h>
#include <basic.h>
#include <num.h>
#include <source.h>
#include <str.h>
#include <type_num.h>
#include <type_resolve.h>
#include <type_sym.h>

static value context_value;

static value dynamic_context(value x)
	{
	{
	/* Define numeric literals. */
	const char *name = ((string)x->R->R)->data;
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value exp = eval(A(A(A(hold(context_value),hold(x)),
		hold(null)),hold(cons)));

	value def;
	if (exp->L && exp->L->L == cons)
		def = hold(exp->L->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

value resolve_in_context(value exp, value context)
	{
	value fun;
	value save = context_value;

	context_value = context;
	fun = resolve(exp,dynamic_context);
	drop(context);

	context_value = save;
	return fun;
	}

/* (resolve label form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	const char *save = source_label;

	value label = f->L->L->R;
	source_label = ((string)label->R->R)->data;

	replace(f, resolve_in_context(hold(f->L->R), hold(f->R)));

	source_label = save;
	return f;
	}
	}
