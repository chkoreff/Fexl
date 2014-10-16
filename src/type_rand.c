#include <value.h>
#include <basic.h>
#include <num.h>
#include <stdlib.h> /* rand */
#include <type_num.h>
#include <type_rand.h>

/* Seed rand with an arbitrary number between 0 and 1. */
value type_seed_rand(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	if (x->T == type_num)
		{
		double seed = *((number)x->R) * (double)RAND_MAX;
		srand(seed);
		f = hold(I);
		}
	else
		f = Q(type_void);
	drop(x);
	return f;
	}
	}

/* Uniform distribution, 0 <= x <= 1 */
value type_rand(value f)
	{
	double x = ((double)rand()) / ((double)RAND_MAX);
	(void)f;
	return Qnum(num_new_double(x));
	}
