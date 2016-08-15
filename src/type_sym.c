#include <value.h>
#include <basic.h>
#include <memory.h>
#include <str.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(symbol sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct symbol));
	}

/*
This type returns 0 so that symbolic forms returned from the parse routine
evaluate properly.
LATER 20160726 Revisit why we see form applied to form, which is why we can't
use type_void here.
*/
value type_sym(value f)
	{
	if (f->N == 0)
		{
		sym_free((symbol)f->R);
		return 0;
		}
	return 0;
	}

value Qsym(string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym);
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	type t = (f->T == type_sym || g->T == type_sym) ? type_sym : type_A;
	return V(t,f,g);
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq((string)x->name->R,(string)y->name->R);
	}

/* Make a pattern that sends the argument to the left and right as needed. */
value fuse(value p, value q)
	{
	if (p->T == type_F && q->T == type_F)
		{
		drop(p);
		drop(q);
		return QF();
		}
	else
		return A(p,q);
	}

/* Replace all occurrences of sym in exp with T, returning a pair with the
replacement pattern and the updated exp. */
static value remove_symbol(value sym, value exp)
	{
	if (exp->T != type_sym)
		return A(QF(),hold(exp));
	else if (exp->L == 0)
		{
		if (sym_eq((symbol)sym->R,(symbol)exp->R))
			return A(QT(),QT());
		else
			return A(QF(),hold(exp));
		}
	else
		{
		value f = remove_symbol(sym,exp->L);
		value g = remove_symbol(sym,exp->R);
		value h = A(
			fuse(hold(f->L),hold(g->L)),
			app(hold(f->R),hold(g->R)));
		drop(f);
		drop(g);
		return h;
		}
	}

/* Return a function that calls subst(p,f,x) when applied to x. */
value Qsubst(value p, value f)
	{
	return app(V(type_subst,Q(type_subst),p),f);
	}

/* Abstract the symbol from exp, returning a form which is a function of that
symbol, and no longer contains that symbol. */
value lam(value sym, value exp)
	{
	value pair = remove_symbol(sym,exp);
	value f = Qsubst(hold(pair->L),hold(pair->R));
	drop(pair);
	drop(sym);
	drop(exp);
	return f;
	}

/* Use pattern p to make a copy of f with x substituted in various places.

I do one level of look-ahead on the left and right patterns, which isn't
strictly necessary but is slightly faster and smaller.

Checking p->R in addition to p->L is redundant, but is slightly faster and
smaller.
*/
static value subst(value p, value f, value x)
	{
	if (p->L && p->R)
		{
		value L = (p->L->T == type_F ? hold(f->L) : subst(p->L,f->L,x));
		value R = (p->R->T == type_F ? hold(f->R) : subst(p->R,f->R,x));
		return A(L,R);
		}
	else if (p->T == type_F)
		return hold(f);
	else
		return hold(x);
	}

/* (subst pattern form value) */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	reduce(f,subst(f->L->L->R,f->L->R,f->R));
	return f;
	}
