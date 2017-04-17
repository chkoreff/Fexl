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
	value name = get_sym(x)->name;
	{
	/* Define numeric literals. */
	value def = Qnum_str0(str_data(name));
	if (def) return def;
	}
	{
	/* Define other names using the given context. */
	value exp = eval(A(A(hold(context),hold(name)),hold(Qyield)));
	value def = hold((exp->L == Qyield) ? exp->R : x);
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
		return resolve_symbol(exp,context);
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

		const char *save_source_label = source_label;
		source_label = str_data(form->label);

		f = resolve(form->exp,context);
		report_undef(f);
		f = V(type_later,hold(QI),f);

		source_label = save_source_label;
		drop(context);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
