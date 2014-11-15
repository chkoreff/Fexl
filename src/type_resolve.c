#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <source.h>
#include <type_num.h>
#include <type_resolve.h>
#include <type_sym.h>

static value context_function;

static value dynamic_context(value x)
	{

	{
	const char *name = ((string)x->R)->data;
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value token_cons = Q(type_cons);
	value exp = eval(A(A(A(hold(context_function),hold(x)),
		hold(C)),token_cons));

	value def;
	if (exp->L && exp->L->L == token_cons)
		def = hold(exp->L->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

/* (resolve label form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return f;
	{
	const char *save_source_label = source_label;
	value save_context_function = context_function;

	value label = f->L->L->R;
	source_label = ((string)label->R)->data;
	context_function = f->R;

	f = resolve(hold(f->L->R),dynamic_context);

	source_label = save_source_label;
	context_function = save_context_function;
	return f;
	}
	}
