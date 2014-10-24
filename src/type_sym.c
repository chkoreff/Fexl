#include <value.h>
#include <basic.h>
#include <context.h>
#include <memory.h>
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
	if (f->N == 0) sym_free((symbol)f->R);
	return f;
	}

value Qsym(short quoted, string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->quoted = quoted ? 1 : 0;
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,sym);
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	value v = A(f,g);
	if (f->T == type_sym || g->T == type_sym)
		v->T = type_sym;
	return v;
	}

static int sym_eq(symbol x, symbol y)
	{
	return x->quoted == y->quoted
		&& str_eq((string)x->name->R,(string)y->name->R);
	}

static value Qsubst(value p, value f)
	{
	return app(A(hold(subst),p),f);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return Qsubst(hold(C),hold(body));
	else if (body->L == 0)
		{
		if (sym_eq((symbol)sym->R, (symbol)body->R))
			return Qsubst(hold(I),hold(I));
		else
			return Qsubst(hold(C),hold(body));
		}
	else
		{
		value f = abstract(sym,body->L);
		value g = abstract(sym,body->R);
		value h = Qsubst(A(hold(f->L->R),hold(g->L->R)),
			app(hold(f->R),hold(g->R)));
		drop(f);
		drop(g);
		return h;
		}
	}

value lam(value sym, value body)
	{
	value f = abstract(sym,body);
	drop(sym);
	drop(body);
	return f;
	}

/* Return the last symbol in the value, if any, in right to left order. */
static value last_sym(value f)
	{
	value sym;
	if (f->T != type_sym) return 0;
	if (f->L == 0) return f;
	sym = last_sym(f->R);
	if (sym) return sym;
	return last_sym(f->L);
	}

value resolve(value f)
	{
	value x = last_sym(f);
	if (x)
		{
		value g = resolve(abstract(x,f));
		value y = current_context(x);
		value h = app(g,y);
		drop(f);
		return h;
		}
	else
		return f;
	}
