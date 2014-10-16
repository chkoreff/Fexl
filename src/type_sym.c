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
	/*LATER possibly more optimization rules */
	if (f == I)
		{
		drop(f);
		return g;
		}
	else
		{
		value v = A(f,g);
		if (f->T == type_sym || g->T == type_sym)
			v->T = type_sym;
		return v;
		}
	}

static int sym_eq(symbol x, symbol y)
	{
	return x->quoted == y->quoted
		&& str_eq((string)x->name->R,(string)y->name->R);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return A(hold(C),hold(body));  /* (\x f) = (C f) */
	else if (body->L == 0)
		{
		if (sym_eq((symbol)sym->R, (symbol)body->R))
			return hold(I);  /* (\x x) = I */
		else
			return app(hold(C),hold(body));  /* (\x y) = (C y) */
		}
	else
		{
		value f = abstract(sym,body->L);
		value g = abstract(sym,body->R);

		value h;
		/* Set the result h = (S f g), optimizing if possible. */
		if (f->L == C)
			{
			if (g == I)
				/* S (C x) I = x */
				h = hold(f->R);
			else if (g->L == C)
				/* S (C x) (C y) = C (x y) */
				h = app(hold(C),app(hold(f->R),hold(g->R)));
			else
				/* S (C x) y = R x y */
				h = app(app(hold(R),hold(f->R)),hold(g));
			}
		else if (g->L == C)
			/* S x (C y) = L x y */
			h = app(app(hold(L),hold(f)),hold(g->R));
		else
			h = app(app(hold(S),hold(f)),hold(g));

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
