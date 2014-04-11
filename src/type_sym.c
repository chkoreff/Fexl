#include <memory.h>
#include <str.h>

#include <value.h>
#include <type.h>
#include <type_str.h>
#include <type_sym.h>

value type_sym(value f, value g)
	{
	if (g) return collect(f,g);
	struct sym *sym = atom_sym(f);
	drop(sym->name);
	free_memory(sym,sizeof(struct sym));
	return 0;
	}

struct sym *atom_sym(value f)
	{
	return (struct sym *)atom_data(f,type_sym);
	}

value Qsym(struct str *name, int line)
	{
	struct sym *sym = new_memory(sizeof(struct sym));
	sym->name = Qstr(name);
	hold(sym->name);
	sym->line = line;
	return atom(type_sym,sym);
	}

value Qsym0(const char *name, int line)
	{
	return Qsym(str_new_data0(name),line);
	}

static int same_sign(int x, int y)
	{
	return (x >= 0 && y >= 0) || (x < 0 && y < 0);
	}

int sym_eq(struct sym *x, struct sym *y)
	{
	return same_sign(x->line,y->line)
		&& str_eq(atom_str(x->name),atom_str(y->name));
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
			return f->R;
		else if (g->L == C)
			/* S (C x) (C y) = C (x y) */
			return app(C,app(f->R,g->R));
		else
			/* S (C x) y = R x y */
			return app(app(R,f->R),g);
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		return app(app(L,f),g->R);
	else
		return app(app(S,f),g);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return A(C,body);
	else if (body->L == 0)
		{
		if (sym_eq(atom_sym(sym),atom_sym(body)))
			return I;  /* (\x x) = I */
		else
			return app(C,body);
		}
	else
		{
		value f = abstract(sym,body->L); hold(f);
		value g = abstract(sym,body->R); hold(g);
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
	value f = abstract(sym,body);
	drop(sym);
	drop(body);
	return f;
	}

/* Return the last symbol in the value, if any, in right to left order. */
static value last_sym(value f)
	{
	if (f->T != type_sym)
		return 0;

	if (f->L == 0)
		return f;

	value x = last_sym(f->R);
	if (x) return x;
	return last_sym(f->L);
	}

value resolve(value f, value context(value))
	{
	value x = last_sym(f);
	if (x == 0) return f;
	value g = resolve(lam(x,f),context);
	value def = context(x);
	return app(g,def);
	}
