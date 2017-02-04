#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(symbol sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct symbol));
	}

value type_sym(value f)
	{
	if (f->N == 0)
		{
		sym_free(get_sym(f));
		return 0;
		}
	return type_void(f);
	}

value Qsym(string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym);
	}

symbol get_sym(value x)
	{
	return (symbol)x->R;
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	type t = (f->T == type_sym || g->T == type_sym) ? type_sym : type_A;
	return V(t,f,g);
	}

/* Return a function that calls subst(p,e,x) when applied to x. */
value Qsubst(value p, value e)
	{
	type t = (e->T == type_sym ? type_sym : type_subst);
	return V(t,V(type_subst,hold(&QI),p),e);
	}

/* Return the equivalent of (C x) */
static value keep(value x)
	{
	return Qsubst(hold(&QF),hold(x));
	}

/* Return the equivalent of I. */
static value here(void)
	{
	return Qsubst(hold(&QT),hold(&QI));
	}

/* Make a pattern that sends the argument to the left and right as needed. */
static value combine(value p, value q)
	{
	if (p == &QF && q == &QF)
		{
		drop(q);
		return p;
		}
	else
		return A(p,q);
	}

/* Return the equivalent of (S x y) */
static value fuse(value x, value y)
	{
	value p = combine(hold(x->L->R),hold(y->L->R));
	value e = app(hold(x->R),hold(y->R));
	drop(x);
	drop(y);
	return Qsubst(p,e);
	}

string sym_name(symbol x)
	{
	return get_str(x->name);
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(sym_name(x),sym_name(y));
	}

/* Abstract the symbol from exp, returning a form which is a function of that
symbol, and no longer contains that symbol. */
value lam(value sym, value exp)
	{
	if (exp->T != type_sym)
		return keep(exp);
	else if (exp->L == 0)
		{
		if (sym_eq(get_sym(sym),get_sym(exp)))
			return here();
		else
			return keep(exp);
		}
	else
		return fuse(lam(sym,exp->L),lam(sym,exp->R));
	}

/* Use pattern p to make a copy of expression e with argument x substituted in
the places designated by the pattern.

I do one level of look-ahead on the left and right patterns, which isn't
strictly necessary but is slightly faster. */
static value subst(value p, value e, value x)
	{
	if (p->L)
		{
		value L = (p->L == &QF ? hold(e->L) : subst(p->L,e->L,x));
		value R = (p->R == &QF ? hold(e->R) : subst(p->R,e->R,x));
		return A(L,R);
		}
	else if (p == &QF)
		return hold(e);
	else
		return hold(x);
	}

/* (subst p e x) Calls substitute. */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return reduce(f,subst(f->L->L->R,f->L->R,f->R));
	}
