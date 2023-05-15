#include <die.h>
#include <memory.h>
#include <stdio.h>
// #include <string.h> // strlen

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <io.h>
#include <show.h>

int argc;
const char **argv;

static unsigned long remain_steps;
static unsigned long max_bytes = 1000000;

static value (*orig_step_app)(value);

static value limit_step_app(value pair)
	{
	if (remain_steps == 0)
		return 0;

	if (cur_bytes > max_bytes)
		{
		// Out of memory
		remain_steps = 0;
		return 0;
		}

	remain_steps--;
	return orig_step_app(pair);
	}

value limit_eval(unsigned long steps, value pair)
	{
	orig_step_app = type_app.step;
	type_app.step = limit_step_app;

	max_bytes = 1000000;
	remain_steps = steps;
	pair = eval(pair);

	type_app.step = orig_step_app;
	return pair;
	}

void try_eval(value exp)
	{
	value pair = A(exp,R(0));
	pair = eval(pair);
	show_line("pair = ",pair);

	printf("cur_bytes = %lu\n\n",cur_bytes);
	drop(pair);
	clear_free_list();
	}

void try_limit_eval(unsigned long steps, value exp)
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

value exp_7811(void)
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
value exp_YI(void)
	{
	return
	A(
		L(A(L(A(R(1),R(0))),L(R(0)))),
		L(A(L(A(R(0),R(0))),L(A(R(1),A(R(0),R(0))))))
	);
	}

// This function consumes an unbounded amount of memory.
// (Y Y)
value exp_YY(void)
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
value exp_YSSS(void)
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

void run_tests(void)
	{
	printf("sizeof(struct value) = %lu\n", sizeof(struct value));

	try_eval(R(0));
	try_eval(I());
	try_eval(II());
	try_eval(exp_7811());
	try_limit_eval(10000,exp_YI());
	try_limit_eval(40000,exp_YY());
	try_limit_eval(40000,exp_YSSS());
	}

void run_script(void)
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
