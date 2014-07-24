#include <value.h>
#include <basic.h>
#include <memory.h>
#include <str.h>
#include <type_str.h>
#include <type_sym.h>

static void sym_free(symbol sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct symbol),3);
	}

value type_sym(value f)
	{
	if (f->N == 0) sym_free((symbol)f->R);
	return 0;
	}

value Qsym(short quoted, value name, unsigned long line)
	{
	if (!name) return 0;
	hold(name);

	{
	symbol sym = new_memory(sizeof(struct symbol),3);
	if (!sym)
		{
		drop(name);
		return 0;
		}

	sym->quoted = quoted ? 1 : 0;
	sym->name = name;
	sym->line = line;

	{
	value f = D(type_sym,sym);
	if (!f) sym_free(sym);
	return f;
	}
	}
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	value v = A(f,g);
	if (!v) return 0;
	if (f->T == type_sym || g->T == type_sym)
		v->T = type_sym;
	return v;
	}

/* Return (S f g), optimizing if possible. */
static value fuse(value f, value g)
	{
	if (f->L == C)
		{
		if (g == I)
			/* S (C x) I = x */
			return f->R;
		else if (g->L == C)
			/* S (C x) (C y) = C (x y) */
			return app(C,app(f->R,g->R));
		else
			{
			/* S (C x) y = R x y */
			return app(app(R,f->R),g);
			}
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		return app(app(L,f),g->R);
	else
		return app(app(S,f),g);
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
		return A(C,body);
	else if (body->L == 0)
		{
		if (sym_eq((symbol)sym->R, (symbol)body->R))
			return I;  /* (\x x) = I */
		else
			return app(C,body);
		}
	else
		{
		value f = hold(abstract(sym,body->L));
		value g = hold(abstract(sym,body->R));
		value h = fuse(f,g);
		drop(f);
		drop(g);
		return h;
		}
	}

value lam(value sym, value body)
	{
	hold(sym);
	hold(body);
	{
	value f = (sym && body) ? abstract(sym,body) : 0;
	drop(sym);
	drop(body);
	return f;
	}
	}
