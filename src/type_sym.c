#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <string.h> /* strcmp */
#include <type_str.h>
#include <type_sym.h>

value Qsubst;
value Qevaluate;

value type_sym(value f)
	{
	return type_atom(f);
	}

static void sym_free(struct symbol *sym)
	{
	drop(sym->name);
	drop(sym->pattern);
	free_memory(sym,sizeof(struct symbol));
	}

value Qsym(value name, unsigned long line, value pattern)
	{
	static struct value atom = {0, (type)sym_free};
	struct symbol *sym = new_memory(sizeof(struct symbol));
	sym->name = name;
	sym->line = line;
	sym->pattern = pattern;
	return V(type_sym,&atom,(value)sym);
	}

value type_form(value f)
	{
	return type_atom(f);
	}

value Qform(value label, value exp)
	{
	static struct value atom = {0, (type)drop};
	return V(type_form,&atom,A(label,exp));
	}

/* Make a reference to a fixed value with no symbols. */
value form_val(value exp)
	{
	return V(0,hold(Qnull),exp);
	}

/* Make a reference to a symbol on a given line. */
value form_ref(value name, unsigned long line)
	{
	return V(0,V(0,Qsym(name,line,hold(QT)),hold(Qnull)),hold(QI));
	}

static struct symbol *top_sym(value x)
	{
	return (x->T == type_null) ? 0 : (struct symbol *)x->L->R;
	}

/* Merge the symbol lists and combine patterns where they intersect. */
static value sym_merge(value fun_list, value arg_list);

static value combine(struct symbol *sym,
	value lp, value rp,
	value lt, value rt)
	{
	return V(0,
		Qsym(hold(sym->name),sym->line,A(hold(lp),hold(rp))),
		sym_merge(lt,rt));
	}

static value sym_merge(value fun_list, value arg_list)
	{
	struct symbol *fun_sym = top_sym(fun_list);
	struct symbol *arg_sym = top_sym(arg_list);
	int cmp;

	if (!fun_sym)
		{
		if (!arg_sym) return hold(Qnull);
		cmp = 1;
		}
	else if (!arg_sym)
		cmp = -1;
	else
		cmp = strcmp(str_data(fun_sym->name),str_data(arg_sym->name));

	if (cmp < 0)
		return combine(fun_sym,
			fun_sym->pattern,QF,
			fun_list->R,arg_list);
	else if (cmp > 0)
		return combine(arg_sym,
			QF,arg_sym->pattern,
			fun_list,arg_list->R);
	else
		return combine(fun_sym,
			fun_sym->pattern,arg_sym->pattern,
			fun_list->R,arg_list->R);
	}

/* Make an applicative form with the given type. */
value form_join(type t, value fun, value arg)
	{
	value result = V(0,
		sym_merge(fun->L,arg->L),
		V(t,hold(fun->R),hold(arg->R)));
	drop(fun);
	drop(arg);
	return result;
	}

/* Apply function to argument, keeping the type of the function. */
value form_appv(value fun, value arg)
	{
	return form_join(fun->R->T,fun,arg);
	}

/* Apply function to argument. */
value form_app(value fun, value arg)
	{
	return form_join(type_A,fun,arg);
	}

/* Delete the symbol with the given name and return the associated pattern. */
static value sym_pop(const char *name, value list, value *pattern)
	{
	struct symbol *sym = top_sym(list);
	int cmp;

	if (!sym)
		cmp = -1;
	else
		cmp = strcmp(name,str_data(sym->name));

	if (cmp < 0)
		{
		*pattern = hold(QF); /* not found */
		return hold(list);
		}
	else if (cmp > 0)
		return V(0,hold(list->L),sym_pop(name,list->R,pattern));
	else
		{
		*pattern = hold(sym->pattern);
		return hold(list->R);
		}
	}

/* Abstract the name from the body. */
value form_lam(value name, value body)
	{
	value pattern;
	value list = sym_pop(str_data(name),(body->L),&pattern);
	value exp = V(0,
		sym_merge(Qnull,list),
		AV(AV(hold(Qsubst),pattern),hold(body->R)));

	drop(list);
	drop(name);
	drop(body);
	return exp;
	}

/* Use pattern p to make a copy of expression e with argument x substituted in
the places designated by the pattern. */
static value subst(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return V(e->T,subst(p->L,e->L,x),subst(p->R,e->R,x));
	}

/* (subst p e x) Calls substitute. */
value type_subst(value f)
	{
	if (!f->L->L->L) return 0;
	return subst(f->L->L->R,f->L->R,f->R);
	}

static value resolve_sym(value context, struct symbol *sym)
	{
	/* "standard" always refers to the current context. */
	if (strcmp(str_data(sym->name),"standard") == 0)
		return hold(context);

	{
	value val = eval(A(A(hold(context),hold(sym->name)),hold(Qcatch)));
	if (val->T == type_catch && val->L && !val->L->R)
		{
		value x = hold(val->R);
		drop(val);
		return x;
		}
	else
		{
		drop(val);
		return 0;
		}
	}
	}

static value resolve(value context, value form)
	{
	value list = form->R->L;
	value exp = hold(form->R->R);
	const char *label = str_data(form->L);
	int undefined = 0;

	while (list->T != type_null)
		{
		struct symbol *sym = top_sym(list);
		value val = resolve_sym(context,sym);
		if (val == 0)
			{
			undefined = 1;
			undefined_symbol(str_data(sym->name),sym->line,label);
			val = hold(Qvoid);
			}

		{
		value next = subst(sym->pattern,exp,val);
		drop(val);
		drop(exp);
		exp = next;
		}

		list = list->R;
		}

	if (undefined)
		die(0); /* The expression had undefined symbols. */

	return exp;
	}

/* (evaluate context exp) Evaluate the expression in the context. */
value type_evaluate(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value context = arg(f->L->R);
	value exp = arg(f->R);
	if (exp->T == type_form)
		f = resolve(context,exp->R);
	else
		f = hold(Qvoid);
	drop(context);
	drop(exp);
	return f;
	}
	}

/* (resolve context exp) Resolve the expression in the context and yield it. */
value type_resolve(value f)
	{
	f = type_evaluate(f);
	if (f) f = AV(hold(Qyield),f);
	return f;
	}
