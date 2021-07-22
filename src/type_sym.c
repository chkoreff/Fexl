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

static void sym_free(struct symbol *sym)
	{
	drop(sym->name);
	drop(sym->pattern);
	free_memory(sym,sizeof(struct symbol));
	}

static struct form *form_new(struct symbol *sym, value exp, value label)
	{
	struct form *form = new_memory(sizeof(struct form));
	form->sym = sym;
	form->exp = exp;
	form->label = label;
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
	if (form->label)
		drop(form->label);
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
	return form_new(0,exp,0);
	}

static struct symbol *sym_new(value name, value pattern, unsigned long line,
	struct symbol *next)
	{
	struct symbol *sym = new_memory(sizeof(struct symbol));
	sym->next = next;
	sym->name = name;
	sym->pattern = pattern;
	sym->line = line;
	return sym;
	}

/* Make a reference to a symbol on a given line. */
struct form *form_ref(string name, unsigned long line)
	{
	return form_new(sym_new(Qstr(name),hold(QT),line,0),hold(QI),0);
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
		cmp = strcmp(str_data(fun->name),str_data(arg->name));

	if (cmp < 0)
		{
		fun->pattern = A(fun->pattern,hold(QF));
		fun->next = sym_merge(fun->next,arg);
		}
	else if (cmp > 0)
		{
		arg->pattern = A(hold(QF),arg->pattern);
		arg->next = sym_merge(fun,arg->next);
		fun = arg;
		}
	else
		{
		fun->pattern = A(fun->pattern,hold(arg->pattern));
		fun->next = sym_merge(fun->next,arg->next);
		sym_free(arg);
		}
	return fun;
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
		cmp = strcmp(name,str_data(sym->name));

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

/* Copy a list of symbols except the one with the given pattern. */
static struct symbol *sym_copy(struct symbol *sym, value pattern)
	{
	if (!sym) return 0;
	if (sym->pattern == pattern)
		return sym_copy(sym->next,pattern);
	return sym_new(hold(sym->name),hold(sym->pattern),sym->line,
		sym_copy(sym->next,pattern));
	}

/* Return true if the form has no undefined symbols. */
value type_is_closed(value f)
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = (struct form *)exp->R;
		f = boolean(form->sym == 0);
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}

/* Define key as val in a form. */
static value def(const char *key, value val, value exp)
	{
	struct form *form = (struct form *)exp->R;
	struct symbol *cur = form->sym;
	value pattern = 0;

	while (cur)
		{
		int cmp = strcmp(key,str_data(cur->name));
		if (cmp > 0)
			cur = cur->next;
		else
			{
			if (cmp == 0)
				pattern = cur->pattern;
			break;
			}
		}

	if (pattern == 0)
		return hold(exp);
	else
		return Qform(form_new(
			sym_copy(form->sym,pattern),
			subst(pattern,form->exp,val),
			hold(form->label)));
	}

value type_def(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value key = arg(f->L->L->R);
	if (key->T == type_str)
		{
		value exp = arg(f->R);
		if (exp->T == type_form)
			f = def(str_data(key),f->L->R,exp);
		else
			f = hold(Qvoid);
		drop(exp);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}
	}

/* Evaluate the form if all symbols are defined, otherwise report the undefined
symbols and die. */
value type_value(value f)
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = (struct form *)exp->R;
		struct symbol *cur = form->sym;
		if (cur == 0)
			f = hold(form->exp);
		else
			{
			const char *label = str_data(form->label);
			while (cur)
				{
				undefined_symbol(str_data(cur->name),cur->line,label);
				cur = cur->next;
				}
			die(0);
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}

/* (resolve context exp) Resolve the expression in the context and yield it. */
value type_resolve(value f)
	{
	f = type_value(f);
	if (f) f = AV(hold(Qyield),f);
	return f;
	}

static struct symbol *resolve(struct symbol *sym, value *result,
	value op(const char *name))
	{
	if (!sym) return 0;
	{
	value val = op(str_data(sym->name));
	if (val)
		{
		value exp = subst(sym->pattern,*result,val);
		drop(val);
		drop(*result);
		*result = exp;
		return resolve(sym->next,result,op);
		}
	else
		return sym_new(hold(sym->name),hold(sym->pattern),
			sym->line,resolve(sym->next,result,op));
	}
	}

value op_resolve(value f, value op(const char *name))
	{
	if (!f->L) return 0;
	{
	value exp = arg(f->R);
	if (exp->T == type_form)
		{
		struct form *form = (struct form *)exp->R;
		if (form->sym == 0)
			f = hold(exp);
		else
			{
			value result = hold(form->exp);
			struct symbol *sym = resolve(form->sym,&result,op);
			f = Qform(form_new(sym,result,hold(form->label)));
			}
		}
	else
		f = hold(Qvoid);
	drop(exp);
	return f;
	}
	}
