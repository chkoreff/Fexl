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
static value abstract(symbol sym, value body)
	{
	if (body->T != type_sym)
		return A(C,body);
	else if (body->L == 0)
		{
		if (sym_eq(sym, (symbol)body->R))
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
	value f = (sym && body) ? abstract((symbol)sym->R,body) : 0;
	drop(sym);
	drop(body);
	return f;
	}
	}

/* Return the last symbol in the value, if any, in right to left order. */
static symbol last_sym(value f)
	{
	if (f->T != type_sym) return 0;
	if (f->L == 0) return (symbol)f->R;
	{
	symbol sym = last_sym(f->R);
	if (sym) return sym;
	return last_sym(f->L);
	}
	}

/*
Extract all symbols from a symbolic form, returning a pure function which can
be used to resolve symbol definitions easily.

For example, consider this source text:

	say "hello"
	say something

The resulting form is:

\form=
	(\sym\exp
	sym F "something" 1 ;
	sym F "say" 2 ;
	sym C "hello" 1 ;
	exp (\something\say\"hello" say "hello" say something)
	)

Each symbol is of the form (sym quoted name line).  The <quoted> flag is C if
the symbol is a string, or F it it's a name.  The <name> is the string name of
the symbol.  The <line> is the line number on which the last occurrence of the
symbol appears.

Note that the form above is a completely specified function with no unresolved
names.  Although it is shown in lambda notation above, in reality it would be
completely reduced to combinator form, i.e.:

	((S ((R R) ((L ((L ((L I) F)) "something")) 2)))
	((S ((R R) ((L ((L ((L I) F)) "say")) 2)))
	((S ((R R) ((L ((L ((L I) C)) "hello")) 1)))
	(C ((L I) ((L ((R S) (L I))) I))))))
*/
value extract_syms(value f)
	{
	symbol sym = last_sym(f);
	if (sym)
		{
		value g = hold(abstract(sym,f));
		value h = extract_syms(g);
		drop(g);

		return A(A(S,A(A(R,R),A(A(L,A(A(L,A(A(L,I),
			Qboolean(sym->quoted))),sym->name)),Qnum_ulong(sym->line)))),
			h);
		}
	else
		return A(C,A(A(L,I),f));
	}
