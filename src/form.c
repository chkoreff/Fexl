#include "str.h"

#include "value.h"
#include "basic.h"
#include "form.h"
#include "long.h"
#include "qstr.h"

value type_form(value f)
	{
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

/* Create a symbol, either a name or a quoted string. */
value symbol(value quoted, struct str *name, long line)
	{
	return V(type_form, Qstr(name), A(quoted,Qlong(line)));
	}

/* Return true if the value is a symbol with a null name. */
int is_null_name(value x)
	{
	return x->T == type_form
		&& x->L->T == type_string
		&& x->R->L->T != fexl_C
		&& get_str(x->L)->len == 0;
	}

/* Return the first symbol in the value, if any, in left to right order. */
value first_symbol(value f)
	{
	if (f->T != type_form) return 0;
	if (f->L->T == type_string) return f;
	value x = first_symbol(f->L);
	return x ? x : first_symbol(f->R);
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
			return apply(C,apply(f->R,g->R));
		else
			/* S (C x) y = R x y */
			return apply(apply(R,f->R),g);
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		return apply(apply(L,f),g->R);
	else
		return apply(apply(S,f),g);
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
value abstract(value sym, value body)
	{
	hold(body);

	value h = 0;
	if (body->T == type_form)
		{
		value f = body->L;
		value g = body->R;

		if (f->T == type_string)
			{
			if (sym->R->L->T == g->L->T
				&& str_cmp(get_str(sym->L),get_str(f)) == 0)
				h = I;  /* (\x x) = I */
			}
		else
			{
			f = abstract(sym,f);
			g = abstract(sym,g);
			h = fuse(f,g); /* (\x F G) = (S (\x F) (\x G)) */
			drop(f);
			drop(g);
			}
		}

	if (h == 0)
		h = apply(C,body);  /* (\x F) = (C F) */

	hold(h);
	drop(body);
	return h;
	}
