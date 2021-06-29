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

static void sym_free(struct symbol *sym)
	{
	str_free(sym->name);
	drop(sym->pattern);
	drop(sym->source);
	free_memory(sym,sizeof(struct symbol));
	}

static struct form *form_new(struct symbol *sym, value exp)
	{
	struct form *form = new_memory(sizeof(struct form));
	form->sym = sym;
	form->exp = exp;
	return form;
	}

static void form_discard(struct form *form)
	{
	free_memory(form,sizeof(struct form));
	}

static void form_free(struct form *form)
	{
	struct symbol *sym = form->sym;
	while (sym)
		{
		struct symbol *next = sym->next;
		sym_free(sym);
		sym = next;
		}
	drop(form->exp);
	form_discard(form);
	}

value type_form(value f)
	{
	return type_atom(f);
	}

value Qform(struct form *exp)
	{
	static struct value atom = {0, (type)form_free};
	return V(type_form,&atom,(value)exp);
	}

/* Make a reference to a fixed value with no symbols. */
struct form *form_val(value exp)
	{
	return form_new(0,exp);
	}

/* Make a reference to a symbol on a given line. */
struct form *form_ref(string name, unsigned long line, value source)
	{
	struct symbol *sym = new_memory(sizeof(struct symbol));
	sym->next = 0;
	sym->name = name;
	sym->pattern = hold(QT);
	sym->line = line;
	sym->source = source;
	return form_new(sym,hold(QI));
	}

/* Merge the symbol lists and combine patterns where they intersect. */
static struct symbol *sym_merge(struct symbol *fun, struct symbol *arg)
	{
	int cmp;
	if (!fun)
		{
		if (!arg) return 0;
		cmp = 1;
		}
	else if (!arg)
		cmp = -1;
	else
		cmp = strcmp(fun->name->data,arg->name->data);

	if (cmp < 0)
		{
		fun->pattern = A(fun->pattern,hold(QF));
		fun->next = sym_merge(fun->next,arg);
		return fun;
		}
	else if (cmp > 0)
		{
		arg->pattern = A(hold(QF),arg->pattern);
		arg->next = sym_merge(fun,arg->next);
		return arg;
		}
	else
		{
		fun->pattern = A(fun->pattern,hold(arg->pattern));
		fun->next = sym_merge(fun->next,arg->next);
		sym_free(arg);
		return fun;
		}
	}

/* Make an applicative form with the given type. */
struct form *form_join(type t, struct form *fun, struct form *arg)
	{
	fun->sym = sym_merge(fun->sym,arg->sym);
	fun->exp = V(t,fun->exp,arg->exp);
	form_discard(arg);
	return fun;
	}

/* Apply function to argument, keeping the type of the function. */
struct form *form_appv(struct form *fun, struct form *arg)
	{
	return form_join(fun->exp->T,fun,arg);
	}

/* Apply function to argument. */
struct form *form_app(struct form *fun, struct form *arg)
	{
	return form_join(type_A,fun,arg);
	}

/* Delete the symbol with the given name and return the associated pattern. */
static struct symbol *sym_pop(const char *name, struct symbol *sym,
	value *pattern)
	{
	int cmp;
	if (!sym)
		cmp = -1;
	else
		cmp = strcmp(name,sym->name->data);

	if (cmp < 0)
		*pattern = hold(QF); /* not found */
	else if (cmp > 0)
		sym->next = sym_pop(name,sym->next,pattern);
	else
		{
		struct symbol *next = sym->next;
		*pattern = hold(sym->pattern);
		sym_free(sym);
		sym = next;
		}
	return sym;
	}

/* Equivalent to (void)sym_merge(0,sym).  Used when no syms on left side. */
static void sym_merge0(struct symbol *sym)
	{
	while (sym)
		{
		sym->pattern = A(hold(QF),sym->pattern);
		sym = sym->next;
		}
	}

/* Abstract the name from the body. */
struct form *form_lam(string name, struct form *body)
	{
	value pattern;
	struct symbol *sym = sym_pop(name->data,body->sym,&pattern);
	sym_merge0(sym);
	body->sym = sym;
	body->exp = AV(AV(hold(Qsubst),pattern),body->exp);
	str_free(name);
	return body;
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

static value resolve_name(value context, string name)
	{
	/* "standard" always refers to the current context. LATER: Deprecate */
	if (strcmp(name->data,"standard") == 0)
		return hold(context);

	{
	value key = Qstr(name);
	value val = eval(A(A(hold(context),hold(key)),hold(Qcatch)));

	key->L = 0;
	key->R = 0;
	drop(key);

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

static value resolve(value context, struct form *form)
	{
	struct symbol *sym = form->sym;
	value exp = hold(form->exp);
	int undefined = 0;

	while (sym)
		{
		value val = resolve_name(context,sym->name);
		if (val == 0)
			{
			undefined = 1;
			undefined_symbol(sym->name->data,sym->line,str_data(sym->source));
			val = hold(Qvoid);
			}

		{
		value next = subst(sym->pattern,exp,val);
		drop(val);
		drop(exp);
		exp = next;
		}

		sym = sym->next;
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
		{
		struct form *form = (struct form *)exp->R;
		f = resolve(context,form);
		}
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
