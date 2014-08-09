#include <value.h>
#include <basic.h>
#include <num.h>
#include <stdlib.h> /* rand */
#include <type_num.h>
#include <type_rand.h>

/* Seed rand with an arbitrary number. */
value type_seed_rand(value f)
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g = 0;
	if (is_atom(type_num,x))
		{
		double seed = *((number)x->R) * (double)RAND_MAX;
		srand(seed);
		g = hold(I);
		}
	drop(x);
	return g;
	}
	}

/* Uniform distribution, 0 <= x <= 1 */
value type_rand(value f)
	{
	double x = ((double)rand()) / ((double)RAND_MAX);
	(void)f;
	return Qnum(num_new_double(x));
	}
