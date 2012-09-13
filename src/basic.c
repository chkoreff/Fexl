#include "value.h"
#include "basic.h"
#include "resolve.h"

/*
\C = (\x\y x)
This is the "constancy function".  It always returns its first argument and
ignores the second.  It also known as "T" because it behaves like the value
"true".  It is also known as "end" because it behaves like the empty list.
*/
static void reduce2_C(value f)
	{
	replace(f,f->L->R);
	}

void reduce_C(value f)
	{
	f->T = reduce2_C;
	}

/*
\F = (\x\y y)
This represents "false", which always returns its second argument.
*/
void reduce_F(value f)
	{
	replace(f,Q(reduce_I));
	}

/*
\I = (\x x)
This is the "identity" function, which simply returns its argument.
*/
void reduce_I(value f)
	{
	replace(f,f->R);
	}

/*
\Y == (\f f (Y f))
This is the "fixpoint" function, which is used to express recursive functions
in a fully closed form with no need for circular references or symbol tables.
*/
void reduce_Y(value f)
	{
	replace_apply(f,f->R,A(f->L,f->R));
	}

/*
\query = (\x\y y x)   # but with x evaluated *before* y.
The query function is used for eager evaluation.  It's also called "?" in the
standard environment.
*/
static void reduce2_query(value f)
	{
	replace_apply(f,f->R,f->L->R);
	}

void reduce_query(value f)
	{
	eval(f->R);
	f->T = reduce2_query;
	}

/*
\pair = (\x\y\p p x y)
This creates a pair of two things.
*/
static void reduce3_pair(value f)
	{
	replace_apply(f,A(f->R,f->L->L),f->L->R);
	}

static void reduce2_pair(value f)
	{
	replace_apply(f,f->L->R,f->R);
	f->T = reduce3_pair;
	}

void reduce_pair(value f)
	{
	f->T = reduce2_pair;
	}

/*
\item = (\head\tail \end\item item head tail)
This creates a list with the first element head, followed by the list tail.
*/

static void reduce3_item(value f)
	{
	replace_apply(f,f->L->L,f->L->R);
	f->T = reduce3_pair;
	}

static void reduce2_item(value f)
	{
	replace_apply(f,f->L->R,f->R);
	f->T = reduce3_item;
	}

void reduce_item(value f)
	{
	f->T = reduce2_item;
	}

/* (yes x A B) = (A x) */
static void reduce3_yes(value f)
	{
	replace_apply(f, f->L->R, f->L->L->R);
	}

static void reduce2_yes(value f)
	{
	f->T = reduce3_yes;
	}

void reduce_yes(value f)
	{
	f->T = reduce2_yes;
	}

/* Replace with true or false based on flag. */
void replace_boolean(value f, int flag)
	{
	replace(f, Q(flag ? reduce_C : reduce_F));
	}

/* \fold == (\fn\z\xs xs z \x\xs \z=(fn z x) fold fn z xs)
It's about 75% faster implemented in C.
*/
static void reduce3_fold(value f)
	{
	value xs = f->R;
	xs = A(A(xs,Q(reduce_C)),Q(reduce_pair));
	hold(xs);
	eval(xs);

	if (xs->T == reduce_C)
		replace(f, f->L->R);
	else if (xs->T == reduce3_pair)
		{
		replace_apply(f, A(f->L->L,A(A(f->L->L->R,f->L->R),xs->L)), xs->R);
		eval(f->L->R);
		}
	else
		type_error();

	drop(xs);
	}

static void reduce2_fold(value f)
	{
	f->T = reduce3_fold;
	}

void reduce_fold(value f)
	{
	f->T = reduce2_fold;
	}

value pair(value x, value y)
	{
	value f = A(x,y);
	f->T = reduce3_pair;
	return f;
	}

value item(value x, value y)
	{
	value f = A(x,y);
	f->T = reduce3_item;
	return f;
	}
