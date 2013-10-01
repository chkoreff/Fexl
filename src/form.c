#include "str.h"

#include "value.h"
#include "basic.h"
#include "form.h"
#include "long.h"
#include "qstr.h"

value type_name(value f)
	{
	return type_string(f);
	}

value Qname(struct str *p)
	{
	return V(type_name,0,(value)p);
	}

value type_form(value f)
	{
	if (!f->N) drop(f->R);
	return f;
	}

/* Apply f to g, returning a form if either one is a form. */
value apply(value f, value g)
	{
	value v = A(f,g);
	if (f->T == type_form || g->T == type_form)
		v->T = type_form;
	return v;
	}

/* Create a symbol of type_name or type_string. */
value symbol(type t, struct str *name, long line)
	{
	value content = V(t,0,(value)name);
	value sym = V(type_form,0,A(content,Qlong(line)));
	hold(sym->R);
	return sym;
	}

/* Return true if the value is a symbol with a null name. */
int is_null_name(value x)
	{
	if (x->T == type_form && !x->L)
		{
		value content = x->R->L;
		if (content->T == type_name)
			return as_str(content)->len == 0;
		}
	return 0;
	}

/* Return the first symbol in the value, if any, in left to right order. */
value first_symbol(value f)
	{
	if (f->T != type_form) return 0;
	if (!f->L) return f;
	value x = first_symbol(f->L);
	return x ? x : first_symbol(f->R);
	}

/* Return (S f g), optimizing if possible. */
static value fuse(value f, value g)
	{
	hold(f);
	hold(g);

	value h = 0;
	if (f->L == C)
		{
		if (g == I)
			/* S (C x) I = x */
			h = f->R;
		else if (g->L == C)
			/* S (C x) (C y) = C (x y) */
			h = apply(C,apply(f->R,g->R));
		else
			/* S (C x) y = R x y */
			h = apply(apply(R,f->R),g);
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		h = apply(apply(L,f),g->R);

	if (h == 0)
		h = apply(apply(S,f),g);

	drop(f);
	drop(g);

	return h;
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
value abstract(value sym, value body)
	{
	hold(sym);
	hold(body);

	value h = 0;
	if (body->T == type_form)
		{
		if (body->L)
			{
			value f = abstract(sym,body->L); hold(f);
			value g = abstract(sym,body->R); hold(g);

			h = fuse(f,g); /* (\x F G) = (S (\x F) (\x G)) */

			drop(f);
			drop(g);
			}
		else
			{
			value xl = sym->R->L;
			value yl = body->R->L;

			if (xl->T == yl->T &&
				str_cmp(as_str(xl),as_str(yl)) == 0)
				h = I;  /* (\x x) = I */
			}
		}

	if (h == 0)
		h = apply(C,body);  /* (\x F) = (C F) */

	drop(sym);
	drop(body);

	return h;
	}
