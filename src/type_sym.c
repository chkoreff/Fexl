#include <value.h>
#include <basic.h>
#include <memory.h>
#include <pattern.h>
#include <str.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(symbol sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct symbol));
	}

value type_sym(value f)
	{
	return type_void(f);
	}

value Qsym(string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym,(type)sym_free);
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	value h = A(f,g);
	if (f->T == type_sym || g->T == type_sym)
		h->T = type_sym;
	return h;
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(data(x->name),data(y->name));
	}

/* Replace all occurrences of sym in exp with I, returning a pair with the
replacement pattern and the updated exp. */
static value remove_symbol(value sym, value exp)
	{
	if (exp->T != type_sym)
		return A(none(),hold(exp));
	else if (exp->L == 0)
		{
		if (sym_eq(data(sym),data(exp)))
			return A(here(),Q(type_I));
		else
			return A(none(),hold(exp));
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
	return app(A(Q(type_subst),p),f);
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

/* (subst pattern form arg) */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	reduce(f,subst(f->L->L->R,f->L->R,f->R));
	return f;
	}
