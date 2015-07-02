#include <value.h>
#include <basic.h>
#include <num.h>
#include <stdlib.h> /* rand */
#include <type_num.h>
#include <type_rand.h>

/* Seed rand with an arbitrary number between 0 and 1. */
value type_seed_rand(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_num)
		{
		double seed = *((number)data(x)) * (double)RAND_MAX;
		srand(seed);
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}

/* rand = {x}, where x is a weakly pseudo-random number with a uniform
distribution over 0 <= x <= 1. */
value type_rand(value f)
	{
	double x = ((double)rand()) / ((double)RAND_MAX);
	(void)f;
	return single(Qnum0(x));
	}
