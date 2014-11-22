#include <value.h>
#include <basic.h>
#include <die.h>
#include <memory.h>
#include <output.h>
#include <source.h>
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
	if (!f->L) return 0;
	replace_void(f);
	return 0;
	}

value Qsym(short quoted, string name, unsigned long line)
	{
	symbol sym = new_memory(sizeof(struct symbol));
	sym->quoted = quoted;
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
	return x->quoted == y->quoted
		&& str_eq((string)x->name->R->R,(string)y->name->R->R);
	}

static value combine_patterns(value p, value q)
	{
	if (p->T == type_C && q->T == type_C)
		{
		drop(p);
		drop(q);
		return hold(C);
		}
	else
		return A(p,q);
	}

static value Qsubst(value p, value f)
	{
	value g = A(hold(I),p);
	g->T = type_subst;
	return app(g,f);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return Qsubst(hold(C),hold(body));
	else if (body->L == 0)
		{
		if (sym_eq((symbol)sym->R->R, (symbol)body->R->R))
			return Qsubst(hold(I),hold(I));
		else
			return Qsubst(hold(C),hold(body));
		}
	else
		{
		value f = abstract(sym,body->L);
		value g = abstract(sym,body->R);
		value h = Qsubst(combine_patterns(hold(f->L->R),hold(g->L->R)),
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

static value x;
static value substitute(value p, value f)
	{
	if (p->T == type_C)
		return hold(f);
	else if (p->T == type_I)
		return hold(x);
	else if (p->L->T == type_C)
		return A(hold(f->L),substitute(p->R,f->R));
	else
		return A(substitute(p->L,f->L),substitute(p->R,f->R));
	}

/* (subst pattern form arg) */
value type_subst(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	x = f->R;
	replace(f,substitute(f->L->L->R,f->L->R));
	return f;
	}

static void undefined_symbol(const char *name, unsigned long line)
	{
	put_to_error();
	put("Undefined symbol "); put(name); put_error_location(line);
	}

static value do_resolve(value exp, value context(value))
	{
	value x = last_sym(exp);
	if (!x) return exp;
	{
	value fun = do_resolve(abstract(x,exp),context);
	symbol sym = (symbol)x->R->R;

	value def;
	if (sym->quoted)
		def = hold(sym->name);
	else
		{
		def = context(sym->name);
		if (!def)
			{
			const char *name = ((string)sym->name->R->R)->data;
			undefined_symbol(name,sym->line);
			def = hold(x);
			}
		}

	fun = app(fun,def);
	drop(exp);
	return fun;
	}
	}

value resolve(value exp, value context(value))
	{
	exp = do_resolve(exp,context);
	if (exp->T == type_sym)
		die(0); /* The expression had undefined symbols. */
	return exp;
	}
