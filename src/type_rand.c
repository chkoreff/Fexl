#include <value.h>

#include <basic.h>
#include <stdlib.h> // srand rand RAND_MAX
#include <type_num.h>
#include <type_rand.h>

// Seed rand with an arbitrary number between 0 and 1.
value type_seed_rand(value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		double seed = x->v_double * (double)RAND_MAX;
		srand(seed);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// rand returns x, where x is a weakly pseudo-random number with a uniform
// distribution over 0 <= x <= 1.
value type_rand(value f)
	{
	return Qnum(((double)rand()) / ((double)RAND_MAX));
	(void)f;
	}
