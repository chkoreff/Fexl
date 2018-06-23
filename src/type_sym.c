#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <standard.h>
#include <string.h> /* strcmp */
#include <type_str.h>
#include <type_sym.h>

struct symbol
	{
	value name;
	unsigned long line;
	value source;
	};

static struct symbol *sym_new(string name, unsigned long line, value source)
	{
	struct symbol *x = new_memory(sizeof(struct symbol));
	x->name = Qstr(name);
	x->line = line;
	x->source = source;
	return x;
	}

static void sym_free(struct symbol *x)
	{
	drop(x->name);
	drop(x->source);
	free_memory(x,sizeof(struct symbol));
	}

static struct symbol *get_sym(value f)
	{
	return (struct symbol *)f->R;
	}

value type_sym(value f)
	{
	if (f->N == 0)
		sym_free(get_sym(f));
	return 0;
	}

value Qsym(string name, unsigned long line, value source)
	{
	return D(type_sym,sym_new(name,line,source));
	}

value sym_name(value f)
	{
	return get_sym(f)->name;
	}

unsigned long sym_line(value f)
	{
	return get_sym(f)->line;
	}

value sym_source(value f)
	{
	return get_sym(f)->source;
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
		if (strcmp(name,str_data(sym_name(exp))) == 0)
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

static value define_name(value context, value name)
	{
	value def = eval(A(hold(context),hold(name)));
	if (def->T == type_void) /* undefined */
		{
		drop(def);
		/* By default, "standard" refers to the current context. */
		if (strcmp(str_data(name),"standard") == 0)
			return hold(context);
		else
			return 0;
		}
	else if (def->T == type_I && def->L)
		{
		/* Optimize (I x) returned by "later" to x. */
		value x = hold(def->R);
		drop(def);
		return x;
		}
	else
		return def;
	}

static int undefined = 0;

static value resolve(value context, value exp)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		value name = sym_name(exp);
		value def = define_name(context,name);
		if (def) return def;
		undefined = 1;
		undefined_symbol(str_data(name),sym_line(exp),
			str_data(sym_source(exp)));
		return hold(exp);
		}
	else
		{
		value L = resolve(context,exp->L);
		value R = resolve(context,exp->R);
		return A(L,R);
		}
	}

/* (evaluate context exp) Evaluate the expression in the context. */
value type_evaluate(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value context = arg(f->L->R);
	value exp = arg(f->R);
	f = resolve(context,exp);
	if (undefined)
		die(0); /* The expression had undefined symbols. */
	drop(context);
	drop(exp);
	return f;
	}
	}

/* (resolve context exp) Resolve the expression in the context for later
evaluation. */
value type_resolve(value f)
	{
	f = type_evaluate(f);
	if (f) f = A(hold(Qlater),f);
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
