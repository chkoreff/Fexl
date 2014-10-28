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

/* (resolve form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L) return f;
	{
	value form = eval(hold(f->L->R));
	if (form->T == type_form)
		{
		value save = context_function;
		context_function = f->R;

		{
		const char *save = source_label;
		source_label = ((string)form->R->L->R)->data;
		f = resolve(hold(form->R->R),dynamic_context);
		source_label = save;
		}

		context_function = save;
		}
	else
		f = Q(type_void);
	drop(form);
	return f;
	}
	}
