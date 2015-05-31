#include <value.h>
#include <pattern.h>

/* A builder is a function that uses pattern p to build a copy of form f with
argument x substituted in various places. */
typedef value (*builder)(value p, value f, value x);

/* Here are some builders that perform some common substitutions. */

static value subst_0(value p, value f, value x)
	{
	(void)p;
	(void)x;
	return hold(f);
	}

static value subst_1(value p, value f, value x)
	{
	(void)p;
	(void)f;
	return hold(x);
	}

static value subst_110(value p, value f, value x)
	{
	return A(subst(p->L,f->L,x),hold(f->R));
	}

static value subst_101(value p, value f, value x)
	{
	return A(hold(f->L),subst(p->R,f->R,x));
	}

static value subst_111(value p, value f, value x)
	{
	return A(subst(p->L,f->L,x),subst(p->R,f->R,x));
	}

/* Make a pattern that ignores the form and returns the argument. */
value none(void) { return Q((type)subst_0); }

/* Make a pattern that returns the form and ignores the argument. */
value here(void) { return Q((type)subst_1); }

/* Make a pattern with the given builder and left and right sides. */
static value P(builder b, value p, value q)
	{
	value r = A(p,q);
	r->T = (type)b;
	return r;
	}

/* Return true if the pattern is none. */
static int is_none(value p)
	{
	return (builder)p->T == subst_0;
	}

/* Make a pattern that sends the argument to the left and right as needed. */
value fuse(value p, value q)
	{
	if (is_none(p))
		{
		if (is_none(q))
			{
			drop(p);
			drop(q);
			return none();
			}
		else
			return P(subst_101,p,q);
		}
	else if (is_none(q))
		return P(subst_110,p,q);
	else
		return P(subst_111,p,q);
	}

/* Use pattern p to make a copy of f with x substituted in various places. */
value subst(value p, value f, value x)
	{
	return ((builder)p->T)(p,f,x);
	}
