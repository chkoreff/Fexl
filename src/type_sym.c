#include <value.h>
#include <basic.h>
#include <die.h>
#include <memory.h>
#include <num.h>
#include <output.h>
#include <source.h>
#include <str.h>
#include <type_num.h>
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
	type t = (f->T == type_sym || g->T == type_sym) ? type_sym : type_A;
	return V(t,f,g);
	}

static int sym_eq(symbol x, symbol y)
	{
	return str_eq(data(x->name),data(y->name));
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

value Qsubst(value p, value f)
	{
	return app(V(type_subst,Q(type_subst),p),f);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value abstract(value sym, value body)
	{
	if (body->T != type_sym)
		return Qsubst(hold(C),hold(body));
	else if (body->L == 0)
		{
		if (sym_eq(data(sym),data(body)))
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

static value cur_context;

static value dynamic_context(value x)
	{
	{
	/* Define numeric literals. */
	const char *name = ((string)data(x))->data;
	value def = Qnum_str0(name);
	if (def) return def;
	}

	{
	/* Define other names using the given context. */
	value single = Q(type_single);
	value exp = eval(A(A(hold(cur_context),hold(x)),single));

	value def;
	if (exp->L == single)
		def = hold(exp->R);
	else
		def = 0;

	drop(exp);
	return def;
	}
	}

static value do_resolve(value exp)
	{
	value x = last_sym(exp);
	if (!x) return exp;
	{
	value fun = do_resolve(abstract(x,exp));
	symbol sym = data(x);
	value def = dynamic_context(sym->name);
	if (!def)
		{
		const char *name = ((string)data(sym->name))->data;
		undefined_symbol(name,sym->line);
		def = hold(x);
		}

	fun = app(fun,def);
	drop(exp);
	return fun;
	}
	}

static value resolve(value exp, value context)
	{
	value save = cur_context;
	cur_context = context;

	exp = do_resolve(exp);
	if (exp->T == type_sym)
		die(0); /* The expression had undefined symbols. */

	drop(context);
	cur_context = save;
	return exp;
	}

/* (resolve label form context) */
value type_resolve(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	{
	const char *save = source_label;

	value label = f->L->L->R;
	source_label = ((string)data(label))->data;

	replace(f, resolve(hold(f->L->R), hold(f->R)));

	source_label = save;
	return f;
	}
	}
