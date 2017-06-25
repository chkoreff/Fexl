#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <type_form.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(struct symbol *sym)
	{
	str_free(sym->name);
	drop(sym->label);
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

value Qsym(string name, unsigned long line, value label)
	{
	struct symbol *sym = new_memory(sizeof(struct symbol));
	sym->name = name;
	sym->line = line;
	sym->label = label;
	return D(type_sym,sym);
	}

struct symbol *get_sym(value x)
	{
	return (struct symbol *)x->R;
	}

string sym_name(value x)
	{
	return get_sym(x)->name;
	}

unsigned long sym_line(value x)
	{
	return get_sym(x)->line;
	}

string sym_label(value x)
	{
	return get_str(get_sym(x)->label);
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
static void abstract(string name, value exp, value *p, value *e)
	{
	if (exp->T != type_sym)
		{
		*p = hold(QF);
		*e = hold(exp);
		}
	else if (exp->L == 0)
		{
		if (str_eq(name,sym_name(exp)))
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
			*p = xp;
			drop(yp);
			}
		else
			*p = A(xp,yp);

		*e = app(xe,ye);
		}
	}

/* Abstract the name from the expression and return a substitution form. */
value lambda(string name, value exp)
	{
	value p,e,f;
	abstract(name,exp,&p,&e);
	f = Qsubst(p,e);
	drop(exp);
	return f;
	}

/* Report all undefined symbols in the expression. */
static void report_undef(value exp)
	{
	if (exp->T != type_sym)
		;
	else if (exp->L == 0)
		{
		const char *name = sym_name(exp)->data;
		const char *label = sym_label(exp)->data;
		undefined_symbol(name,sym_line(exp),label);
		}
	else
		{
		report_undef(exp->L);
		report_undef(exp->R);
		}
	}

static value check_undef(value f)
	{
	if (f->T == type_sym)
		{
		report_undef(f);
		die(0); /* The expression had undefined symbols. */
		drop(f);
		f = hold(Qvoid);
		}
	return f;
	}

/* (evaluate form) Evaluate the form if it is fully resolved, otherwise report
all the symbols as undefined and die. */
value type_evaluate(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		f = check_undef(hold(form_exp(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (resolved form) Return the form expression if it is fully resolved,
otherwise report all the symbols as undefined and die. */
value type_resolved(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		f = A(Q(type_later),check_undef(hold(form_exp(x))));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (is_resolved form) Return true if the form is fully resolved (contains no
symbols). */
value type_is_resolved(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		f = boolean(form_exp(x)->T != type_sym);
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (define name def form) Define name as def in form. */
value type_define(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	value name = arg(f->L->L->R);
	if (name->T == type_str)
		{
		value form = arg(f->R);
		if (form->T == type_form)
			{
			value def = hold(f->L->R);
			value exp = hold(form_exp(form));
			exp = lambda(get_str(name),exp);
			f = Qform(app(exp,def));
			}
		else
			f = hold(Qvoid);
		drop(form);
		}
	else
		f = hold(Qvoid);
	drop(name);
	return f;
	}
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
