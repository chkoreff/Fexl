#include "value.h"
#include "basic.h"
#include "eval.h"
#include "resolve.h"

/*
\C = (\x\y x)
This is the "constancy function", or "Konstanzfunktion" in the original German.

This represents "true", which always returns its first argument.  It's also
called "T" in the standard context.

It also represents the empty list, which is called "end" in the standard
context.
*/
value fexl_C(value f)
	{
	if (!f->L->L) return f;
	return f->L->R;
	}

/*
\F = (\x\y y)
This represents "false", which always returns its second argument.
*/
value fexl_F(value f)
	{
	if (!f->L->L) return f;
	return f->R;
	}

/*
\S = (\x\y\z (x z) (y z))
This is the "fusion function", or "Verschmelzungfunktion" in the original
German.
*/
value fexl_S(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( A(f->L->L->R, f->R), A(f->L->R, f->R) );
	}

/*
\I = (\x x)
This is the "identity function."
*/
value fexl_I(value f)
	{
	return f->R;
	}

/*
\R = (\x\y\z x (y z))
This is the "composition" function.
I call it R because it passes z to the right side only.
*/
value fexl_R(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( f->L->L->R, A(f->L->R, f->R) );
	}

/*
\L = (\x\y\z x z y)
This is the "swap" function.
I call it L because it passes z to the left side only.
*/
value fexl_L(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A( A(f->L->L->R, f->R), f->L->R);
	}

/*
\Y == (\f f (Y f))
This is the "fixpoint" function, which is used to express recursive functions
in a fully closed form with no need for circular references or symbol tables.
*/
value fexl_Y(value f)
	{
	return A(f->R, A(f->L,f->R));
	}

/*
\query = (\x\y y x)   # but with x evaluated *before* y.
The query function is used for eager evaluation.  It's also called "?" in the
standard environment.
*/
value fexl_query(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	eval(x);
	return A(f->R,x);
	}

/*
\item = (\head\tail \end\item item head tail)
This creates a list with the first element head, followed by the list tail.
*/
value fexl_item(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	return A(A(f->R,f->L->L->L->R),f->L->L->R);
	}

/*
\pair = (\x\y\p p x y)
This creates a pair of two things.
*/
value fexl_pair(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;
	return A(A(f->R,f->L->L->R),f->L->R);
	}
