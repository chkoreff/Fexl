#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <standard.h>
#include <string.h> /* strcmp */
#include <type_str.h>
#include <type_sym.h>

struct symbol
	{
	value name;
	unsigned long line;
	};

static struct symbol *get_sym(value x)
	{
	return (struct symbol *)x->R;
	}

static void sym_free(struct symbol *sym)
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
	struct symbol *sym = new_memory(sizeof(struct symbol));
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym);
	}

const char *sym_name(value x)
	{
	return str_data(get_sym(x)->name);
	}

unsigned long sym_line(value x)
	{
	return get_sym(x)->line;
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

/* Abstract a name from an expression.  Sets *p to the pattern where the name
appears in the expression.  Sets *e to the expression with QI substituted
wherever the name occurred. */
static void abstract(const char *name, value exp, value *p, value *e)
	{
	if (exp->T != type_sym)
		{
		*p = hold(QF);
		*e = hold(exp);
		}
	else if (exp->L == 0)
		{
		if (strcmp(name,sym_name(exp)) == 0)
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

		abstract(name,exp->L,&xp,&xe);
		abstract(name,exp->R,&yp,&ye);

		if (xp == QF && yp == QF)
			{
			drop(xe);
			drop(ye);
			drop(yp);

			*p = QF;
			*e = hold(exp);
			}
		else
			{
			*p = A(xp,yp);
			*e = app(xe,ye);
			}
		}
	}

/* Abstract the name from the expression and return a substitution form. */
value lambda(const char *name, value exp)
	{
	value p,e,f;
	abstract(name,exp,&p,&e);
	f = Qsubst(p,e);
	drop(exp);
	return f;
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
