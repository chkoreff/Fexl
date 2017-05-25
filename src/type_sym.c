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

string sym_name(symbol x)
	{
	return get_str(x->name);
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(sym_name(x),sym_name(y));
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
	return V(t,V(type_subst,hold(QI),p),e);
	}

/* Abstract a symbol from an expression.  Sets *p to the pattern where the
symbol appears in the expression.  Sets *e to the expression with QI
substituted wherever the symbol occurred. */
static void abstract(value sym, value exp, value *p, value *e)
	{
	if (exp->T != type_sym)
		{
		*p = hold(QF);
		*e = hold(exp);
		}
	else if (exp->L == 0)
		{
		if (sym_eq(get_sym(sym),get_sym(exp)))
			{
			*p = hold(QT);
			*e = hold(QI);
			}
		else
			{
			*p = hold(QF);
			*e = hold(exp);
			}
		}
	else
		{
		value xp, xe;
		value yp, ye;

		abstract(sym,exp->L,&xp,&xe);
		abstract(sym,exp->R,&yp,&ye);

		if (xp == QF && yp == QF)
			{
			*p = xp;
			drop(yp);
			}
		else
			*p = A(xp,yp);

		*e = app(xe,ye);
		}
	}

value lam(value sym, value exp)
	{
	value p,e;
	abstract(sym,exp,&p,&e);
	return Qsubst(p,e);
	}

/* Use pattern p to make a copy of expression e with argument x substituted in
the places designated by the pattern. */
static value subst(value p, value e, value x)
	{
	if (p == QF) return hold(e);
	if (p == QT) return hold(x);
	return A(subst(p->L,e->L,x),subst(p->R,e->R,x));
	}

/* (subst p e x) Calls substitute. */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return subst(f->L->L->R,f->L->R,f->R);
	}
