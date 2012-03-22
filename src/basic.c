#include "value.h"
#include "basic.h"
#include "stack.h"

/*
The C function follows the rule ((C x) y) = x.  This is known as the "constancy
function", or "Konstanzfunktion" in the original German.
*/
struct value *type_C(struct value *f)
	{
	if (!f->L->L) return f;
	return f->L->R;
	}

/*
The S function follows the rule (((S x) y) z) = ((x z) (y z)).  This is known
as the "fusion function", or "Verschmelzungfunktion" in the original German.
*/
struct value *type_S(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( A(f->L->L->R, f->R), A(f->L->R, f->R) );
	}

/*
The I function follows the rule (I x) = x.  This is known as the "identity
function."
*/
struct value *type_I(struct value *f)
	{
	return f->R;
	}

/*
The R function follows the rule (R x y z) = (x (y z)).  I call it R because it
passes z to the right side only.  It can be defined as
	S (C S) C
but it's such a common pattern that I make a special combinator for it.
*/
struct value *type_R(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( f->L->L->R, A(f->L->R, f->R) );
	}

/*
The L function follows the rule (L x y z) = (x z y).  I call it L because it
passes z to the left side only.  It can be defined as
	S (S (C S) (S (C C) S)) (C C)
but it's such a common pattern that I make a special combinator for it.
*/
struct value *type_L(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( A(f->L->L->R, f->R), f->L->R);
	}

/*
The Y function follows the rule (Y f) = (f (Y f)).  This is known as the
"fixpoint function."
*/
struct value *type_Y(struct value *f)
	{
	return A(f->R, A(f->L,f->R));
	}

/*
The query function is used for eager evaluation.  It follows this rule:
  (query x y) = (y x)

However, it evaluates x first before passing it to y.

This function is called "?" in the standard context.
*/
struct value *type_query(struct value *f)
	{
	if (!f->L->L) return f;
	if (!f->L->R->T) push(f->L->R);
	return A(f->R,f->L->R);
	}

/* Return a copy of fun with val substituted according to pattern p. */
struct value *subst(struct value *p, struct value *fun, struct value *val)
	{
	if (p->T == type_I) return val;
	if (p->T == type_C) return fun;
	return A(subst(p->L,fun->L,val),subst(p->R,fun->R,val));
	}

struct value *type_lambda(struct value *f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return subst(f->L->L->R,f->L->R,f->R);
	}

struct value *C;
struct value *S;
struct value *I;
struct value *R;
struct value *L;
struct value *Y;
struct value *F;
struct value *query;
struct value *lam;

struct value *get_pattern(struct value *sym, struct value *fun)
	{
	if (fun == sym) return I;
	if (!fun->L) return C;

	struct value *fl = get_pattern(sym,fun->L);
	struct value *fr = get_pattern(sym,fun->R);
	if (fl->T == type_C && fr->T == type_C) return C;

	return A(fl,fr);
	}

struct value *lambda(struct value *x, struct value *f)
	{
	return A(A(lam,get_pattern(x,f)),f);
	}

void beg_basic(void)
	{
	C = Q(type_C);
	S = Q(type_S);
	I = Q(type_I);
	R = Q(type_R);
	L = Q(type_L);
	Y = Q(type_Y);
	query = Q(type_query);
	F = A(C,I);
	lam = Q(type_lambda);

	hold(C);
	hold(S);
	hold(I);
	hold(R);
	hold(L);
	hold(Y);
	hold(query);
	hold(F);
	hold(lam);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(R);
	drop(L);
	drop(Y);
	drop(query);
	drop(F);
	drop(lam);
	}
