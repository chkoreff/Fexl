#include <value.h>
#include <basic.h>
#include <memory.h>
#include <num.h>
#include <str.h>
#include <type_num.h>
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

value Qsym(short quoted, string name, unsigned long line)
	{
	if (!name) return 0;
	{
	symbol sym = new_memory(sizeof(struct symbol),3);
	if (!sym)
		{
		str_free(name);
		return 0;
		}

	sym->quoted = quoted ? 1 : 0;
	sym->name = hold(Qstr(name));
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
		value h = (f && g) ? fuse(f,g) : 0;
		drop(f);
		drop(g);
		return h;
		}
	}

value lam(value sym, value body)
	{
	value f = (sym && body) ? abstract(sym,body) : 0;
	drop(sym);
	drop(body);
	return f;
	}

/* Return the last symbol in the value, if any, in right to left order. */
static value last_sym(value f)
	{
	value sym;
	if (!f || f->T != type_sym) return 0;
	if (f->L == 0) return f;
	sym = last_sym(f->R);
	if (sym) return sym;
	return last_sym(f->L);
	}

value resolve(
	value f,
	value context(const char *name, unsigned long line)
	)
	{
	value x = last_sym(f);
	if (x)
		{
		value g = hold(abstract(x,f));
		value h = hold(resolve(g,context));
		value def;
		{
		symbol sym = x ? (symbol)x->R : 0;
		if (sym->quoted)
			def = sym->name;
		else
			{
			symbol sym = x ? (symbol)x->R : 0;
			string str = (string)sym->name->R;
			const char *name = str->data;
			def = context(name,sym->line);
			/*TODO figure out partial resolution within fexl code */
			if (!def) def = x;
			}
		}
		{
		value result = app(h,def);
		drop(h);
		drop(g);
		return result;
		}
		}
	else
		return f;
	}
