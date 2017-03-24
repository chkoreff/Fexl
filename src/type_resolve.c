#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <report.h>
#include <type_num.h>
#include <type_resolve.h>
#include <type_form.h>
#include <type_str.h>
#include <type_sym.h>

/* Resolve an individual symbol with the context. */
static value resolve_symbol(value x, value context)
	{
	{
	/* Define numeric literals. */
	const char *name = str_data(x);
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value exp = eval(A(A(hold(context),hold(x)),hold(&Qyield)));
	value def;

	if (exp->L == &Qyield)
		def = hold(exp->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

/* Resolve all symbols in exp with the definitions given by context. */
static value resolve(value exp, value context)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		symbol sym = get_sym(exp);
		value def = resolve_symbol(sym->name, context);
		if (!def) def = hold(exp);
		return def;
		}
	else
		{
		value f = resolve(exp->L,context);
		value g = resolve(exp->R,context);
		return app(f,g);
		}
	}

/* Return the first undefined symbol in the expression. */
static value first_undef(value exp)
	{
	if (exp->T != type_sym)
		return 0;
	else if (exp->L == 0)
		return exp;
	else
		{
		value x = first_undef(exp->L);
		if (x) return x;
		return first_undef(exp->R);
		}
	}

/* Report all distinct undefined symbols in the expression. */
static void report_undef(value exp)
	{
	short undefined = 0;
	hold(exp);
	while (1)
		{
		value x = first_undef(exp);
		if (x == 0) break;
		{
		value next;
		symbol sym = get_sym(x);
		const char *name = str_data(sym->name);
		undefined_symbol(name,sym->line);
		undefined = 1;
		next = lam(x,exp);
		drop(exp);
		exp = next;
		}
		}
	if (undefined)
		die(0); /* The expression had undefined symbols. */
	drop(exp);
	}

/* (resolve context form) Resolve the form in the context and return the
resulting function. */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		{
		form form = get_form(x);
		value context = arg(f->L->R);
		value exp;

		const char *save_source_label = source_label;
		source_label = str_data(form->label);

		exp = resolve(form->exp,context);
		report_undef(exp);
		f = V(type_later,hold(&QI),exp);

		source_label = save_source_label;
		drop(context);
		}
	else
		f = hold(&Qvoid);
	drop(x);
	return f;
	}
	}
