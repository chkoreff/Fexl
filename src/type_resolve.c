#include <value.h>
#include <basic.h>
#include <die.h>
#include <num.h>
#include <report.h>
#include <str.h>
#include <type_num.h>
#include <type_sym.h>
#include <type_resolve.h>

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
	value exp = eval(A(A(hold(context),hold(x)),Q(type_single)));
	value def;

	if (exp->L && exp->L->T == type_single)
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
		return exp;
	{
	value result;
	if (exp->L == 0)
		{
		symbol sym = data(exp);
		value x = sym->name;
		result = resolve_symbol(x, context);
		if (!result)
			{
			const char *name = ((string)data(x))->data;
			undefined_symbol(name,sym->line);
			undefined = 1;
			result = Q(type_void);
			}
		}
	else
		{
		value f = do_resolve(hold(exp->L),context);
		value g = do_resolve(hold(exp->R),context);
		result = A(f,g);
		}

	drop(exp);
	return result;
	}
	}

/* Resolve all symbols in exp with the definitions given by context. */
static value resolve(value exp, value context)
	{
	exp = do_resolve(exp,context);
	if (undefined)
		die(0); /* The expression had undefined symbols. */

	drop(context);
	return exp;
	}

/* (resolve label form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	const char *save = source_label;
	string label = data(f->L->L->R);
	source_label = label->data;

	reduce(f, resolve(hold(f->L->R), hold(f->R)));

	source_label = save;
	return f;
	}
	}
