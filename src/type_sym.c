#include <value.h>
#include <basic.h>
#include <memory.h>
#include <str.h>
#include <type_str.h>
#include <type_sym.h>

void sym_free(struct sym *sym)
	{
	drop(sym->name);
	free_memory(sym,sizeof(struct sym));
	}

void type_sym(value f)
	{
	(void)f;
	}

value Qsym(int quoted, string name, unsigned long line)
	{
	struct sym *sym = new_memory(sizeof(struct sym));
	sym->quoted = quoted ? 1 : 0;
	sym->name = Qstr(name);
	sym->line = line;
	return D(type_sym,(type)sym_free,(value)sym);
	}

struct sym *get_sym(value f)
	{
	return (struct sym *)f->R->R;
	}

int sym_eq(struct sym *x, struct sym *y)
	{
	return x->quoted == y->quoted
		&& str_eq(get_str(x->name),get_str(y->name));
	}

/* Apply f to g, where either can be a symbolic form. */
value app(value f, value g)
	{
	value v = A(f,g);
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
			return hold(f->R);
		else if (g->L == C)
			/* S (C x) (C y) = C (x y) */
			return app(hold(C),app(hold(f->R),hold(g->R)));
		else
			/* S (C x) y = R x y */
			return app(app(hold(R),hold(f->R)),hold(g));
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		return app(app(hold(L),hold(f)),hold(g->R));
	else
		return app(app(hold(S),hold(f)),hold(g));
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return A(hold(C),hold(body));
	else if (body->L == 0)
		{
		if (sym_eq(get_sym(sym),get_sym(body)))
			return hold(I);  /* (\x x) = I */
		else
			return app(hold(C),hold(body));
		}
	else
		{
		value f = abstract(sym,body->L);
		value g = abstract(sym,body->R);
		value h = fuse(f,g);
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

value resolve(value f, value context(value))
	{
	value sym = last_sym(f);
	value g, def;
	if (sym == 0) return f;
	g = resolve(abstract(sym,f),context);
	def = context(sym);
	g = app(g,def);
	drop(f);
	return g;
	}
