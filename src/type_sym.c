#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <report.h>
#include <standard.h>
#include <type_form.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(struct symbol *sym)
	{
	drop(sym->name);
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
	sym->name = Qstr(name);
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
	return get_str(get_sym(x)->name);
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
value lambda(string name, value exp)
	{
	value p,e,f;
	abstract(name,exp,&p,&e);
	f = Qsubst(p,e);
	drop(exp);
	return f;
	}

/* Return an expression where each symbol in the original is replaced with the
definition given by resolve_name.  If a symbol has no definition, it remains
unchanged in the result. */
static value resolve(value resolve_name(value name), value exp)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		value def = resolve_name(get_sym(exp)->name);
		if (def)
			return def;
		else
			return hold(exp);
		}
	else
		{
		value L = resolve(resolve_name,exp->L);
		value R = resolve(resolve_name,exp->R);
		if (L == exp->L && R == exp->R)
			{
			drop(L);
			drop(R);
			return hold(exp);
			}
		else
			return app(L,R);
		}
	}

value op_resolve(value resolve_name(value name), value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		f = Qform(resolve(resolve_name,form_exp(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Return an expression where each occurrence of name is replaced with def. */
static value define(string name, value def, value exp)
	{
	if (exp->T != type_sym)
		return hold(exp);
	else if (exp->L == 0)
		{
		if (str_eq(name,sym_name(exp)))
			return hold(def);
		else
			return hold(exp);
		}
	else
		{
		value L = define(name,def,exp->L);
		value R = define(name,def,exp->R);
		if (L == exp->L && R == exp->R)
			{
			drop(L);
			drop(R);
			return hold(exp);
			}
		else
			return app(L,R);
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

/* If the expression has symbols, report each distinct symbol as undefined and
then die. */
static value check_undef(value exp)
	{
	if (exp->T != type_sym)
		return hold(exp);

	hold(exp);
	while (1)
		{
		value x = first_undef(exp);
		if (x == 0) break;
		{
		value old = exp;
		exp = define(sym_name(x),QI,exp);
		drop(old);
		undefined_symbol(sym_name(x)->data,sym_line(x),sym_label(x)->data);
		}
		}

	drop(exp);
	exp = hold(Qvoid);
	die(0); /* The expression had undefined symbols. */
	return exp;
	}

/* (evaluate form) Evaluate the form if it is fully resolved, otherwise report
all the symbols as undefined and die. */
value type_evaluate(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_form)
		f = check_undef(form_exp(x));
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
		f = A(Q(type_later),check_undef(form_exp(x)));
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
			value def = f->L->R;
			value exp = define(get_str(name),def,form_exp(form));
			f = Qform(exp);
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
