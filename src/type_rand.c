#include <num.h>
#include <value.h>

#include <basic.h>
#include <stdlib.h> /* srand rand RAND_MAX */
#include <type_num.h>
#include <type_rand.h>

/* Seed rand with an arbitrary number between 0 and 1. */
value type_seed_rand(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		double seed = get_double(x) * (double)RAND_MAX;
		srand(seed);
		f = hold(&QI);
		}
	else
		f = reduce_void(f);
	drop(x);
	return f;
	}
	}

/* rand returns x, where x is a weakly pseudo-random number with a uniform
distribution over 0 <= x <= 1. */
value type_rand(value f)
	{
	double x = ((double)rand()) / ((double)RAND_MAX);
	(void)f;
	return Qnum0(x);
	}
