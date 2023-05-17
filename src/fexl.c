#include <memory.h>
#include <stdio.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <show.h>

int argc;
const char **argv;

static unsigned long remain_steps;
static unsigned long max_bytes;

static value eval_limit(value pair)
	{
	while (1)
		{
		if (remain_steps == 0)
			break;
		else if (cur_bytes > max_bytes)
			{
			// Out of memory
			remain_steps = 0;
			break;
			}
		else
			{
			value next;
			remain_steps--;
			next = pair->app.fun->type->step(pair);
			if (next == 0) break;
			drop(pair);
			pair = next;
			}
		}
	return pair;
	}

static value limit_eval(unsigned long steps, value pair)
	{
	value (*save_eval)(value f) = eval;
	eval = eval_limit;
	remain_steps = steps;
	max_bytes = 1000000;

	pair = eval(pair);

	eval = save_eval;
	return pair;
	}

static void try_eval(value exp)
	{
	value pair = A(exp,R(0));
	pair = eval(pair);
	show_line("pair = ",pair);

	printf("cur_bytes = %lu\n\n",cur_bytes);
	drop(pair);
	clear_free_list();
	}

static void try_limit_eval(unsigned long steps, value exp)
	{
	value pair = A(exp,R(0));
	pair = limit_eval(steps,pair);

	// NOTE: You can continue an interrupted eval like this.
	if (remain_steps == 0)
		{
		printf("continue\n");
		pair = limit_eval(6,pair);
		//pair = limit_eval(3,pair);
		}

	show_line("pair = ",pair);

	if (remain_steps == 0)
		printf("stopped\n");

	printf("cur_bytes = %lu\n\n",cur_bytes);
	drop(pair);
	clear_free_list();
	}

value I(void)
	{
	return L(R(0));
	}

value II(void)
	{
	return A(I(),I());
	}

static value exp_7811(void)
	{
	value exp = A(II(),II());
	exp = A(exp,hold(exp));
	exp = A(exp,hold(exp));
	exp = A(exp,hold(exp));
	return exp;
	}

// This function runs forever in fixed memory.
// \Y=(\f (\Q Q Q) (\x f (x x))) # fixpoint
// \I=(\x x) # identity
// (Y I)
static value exp_YI(void)
	{
	return
	A(
		L(A(L(A(R(1),R(0))),L(R(0)))),
		L(A(L(A(R(0),R(0))),L(A(R(1),A(R(0),R(0))))))
	);
	}

// This function consumes an unbounded amount of memory.
// (Y Y)
static value exp_YY(void)
	{
	return
	A(
		L(A(R(0),R(0))),
		L(A(L(A(R(0),R(0))),L(A(R(1),A(R(0),R(0)))))
		)
	);
	}

// This function consumes an unbounded amount of memory.
// \S=(\x\y\z x z; y z) # fusion
// Y S S S  # Apply fixpoint to the fusion operator in a weird way.
static value exp_YSSS(void)
	{
	return
	A(
		L(A(
			L(A(A(A(R(0),R(1)),R(1)),R(1))),
			L(A(L(A(R(0),R(0))),L(A(R(1),A(R(0),R(0))))))
			)),
		L(L(L(A(A(R(2),R(0)),A(R(1),R(0))))))
	);
	}

static void run_tests(void)
	{
	printf("sizeof(struct value) = %lu\n", sizeof(struct value));

	try_eval(R(0));
	try_eval(I());
	try_eval(II());
	try_eval(exp_7811());
	try_limit_eval(10000,exp_YI());
	try_limit_eval(80000,exp_YY());
	try_limit_eval(80000,exp_YSSS());
	}

static void run_script(void)
	{
	// TODO run an actual script
	run_tests();
	}

int main(int _argc, const char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	run_script();

	clear_free_list();
	end_memory();
	return 0;
	}
