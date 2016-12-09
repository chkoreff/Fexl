#include <num.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <die.h>
#include <report.h>
#include <type_num.h>
#include <type_resolve.h>
#include <type_sym.h>

/* Resolve an individual symbol with the context. */
static value resolve_symbol(value x, value context)
	{
	{
	/* Define numeric literals. */
	const char *name = ((string)data(x))->data;
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value exp = eval(A(A(hold(context),hold(x)),Q(type_yield)));
	value def;

	if (exp->L && exp->L->T == type_yield)
		def = hold(exp->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

static short undefined = 0;

/* Resolve all symbols in exp with the context. */
static value do_resolve(value exp, value context)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		symbol sym = data(exp);
		value x = sym->name;
		value def = resolve_symbol(x, context);
		if (!def)
			{
			const char *name = ((string)data(x))->data;
			undefined_symbol(name,sym->line);
			undefined = 1;
			def = Qvoid();
			}
		return def;
		}
	else
		{
		value f = do_resolve(exp->L,context);
		value g = do_resolve(exp->R,context);
		return A(f,g);
		}
	}

/* Resolve all symbols in exp with the definitions given by context. */
static value resolve(value exp, value context)
	{
	exp = do_resolve(exp,context);
	if (undefined)
		die(0); /* The expression had undefined symbols. */
	return exp;
	}

/* (resolve context form) Resolve the form in the context and yield the
resulting function. */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value form = arg(f->R);
	if (form->T == type_sym)
		{
		value label = form->L;
		value exp = form->R;
		value context = f->L->R;

		const char *save_source_label = source_label;
		source_label = ((string)data(label))->data;

		reduce(f,yield(resolve(exp,context)));

		source_label = save_source_label;
		}
	else
		reduce_void(f);
	drop(form);
	return f;
	}
	}
