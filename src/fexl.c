#include <die.h>
#include <limit.h>
#include <memory.h>
#include <signal.h> // sigaction
#include <stdio.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <basic.h>

#include <str.h>
#include <buffer.h>
#include <stream.h>
#include <parse.h>
#include <type_num.h>
#include <type_str.h>
#include <type_output.h>

#include <show.h>

static FILE *cur_fh;

static int get(void)
	{
	//return '('; // Test stack overflow in parser.
	return fgetc(cur_fh);
	}

value parse_script(const char *name, value cx_env)
	{
	value exp;
	cur_name = name;
	cur_fh = name[0] ? fopen(name,"r") : stdin;
	if (!cur_fh)
		{
		fprintf(stderr,"Could not open source file %s\n",name);
		die(0);
		}
	cur_get = get;
	skip();

	exp = parse_fexl(cx_env);

	fclose(cur_fh);
	cur_fh = 0;
	return exp;
	}

int argc;
const char **argv;

static void die_perror(const char *msg)
	{
	perror(msg);
	die(0);
	}

static void handle_signal(int signum)
	{
	if (signum == SIGXCPU)
		die("Out of time");
	else if (signum == SIGSEGV)
		die("Out of stack");
	}

/* Reference:
https://stackoverflow.com/questions/31784823/interrupting-open-with-sigalrm

This sets a signal handler so it does not kill the process when the signal
happens, but instead interrupts any system call in progress.
*/
void set_handler(int signum)
	{
	struct sigaction sa;
	sa.sa_handler = handle_signal;
	// Override default SA_RESTART.
	sa.sa_flags = SA_ONSTACK; // Use the alternate signal stack.

	sigemptyset(&sa.sa_mask);
	if (sigaction(signum, &sa, NULL) < 0)
		die_perror("sigaction(2) error");
	}

static stack_t alt_stack;

void init_signal(void)
	{
	// Set alternate stack context for signals.
	{
	size_t num_bytes = 16384;
	alt_stack.ss_sp = new_memory(num_bytes);
	alt_stack.ss_flags = 0;
	alt_stack.ss_size = num_bytes;
	sigaltstack(&alt_stack,NULL);
	}

	// Set signal handlers.
	set_handler(SIGXCPU);
	set_handler(SIGSEGV);
	}

// Benchmark version which counts eval calls.
static unsigned long cur_steps = 0;

static value (*save_step_app)(value);

static value count_step_app(value pair)
	{
	cur_steps++;
	return save_step_app(pair);
	}

static void run_script(value cx_env)
	{
	const char *name = argc > 1 ? argv[1] : "";
	value pair = parse_script(name,cx_env);
	show_line("exp = ",pair->L);
	clear_free_list();

	{
	unsigned long beg_steps = cur_steps;
	unsigned long beg_bytes = cur_bytes;

	save_step_app = type_app.step;
	type_app.step = count_step_app;

	pair = eval(pair);

	type_app.step = save_step_app;

	show_line("exp = ",pair->L);
	{
	unsigned long num_steps = cur_steps - beg_steps;
	unsigned long num_bytes = cur_bytes - beg_bytes;
	printf("num_steps = %lu\n",num_steps);
	printf("num_bytes = %lu\n\n",num_bytes);
	}
	}
	drop(pair);
	}

// Define standard context.
static value cx_std;

static void define(const char *key, value val)
	{
	cx_std = A(A(Qstr0(key),val),cx_std);
	}

static void beg_std(void)
	{
	cx_std = hold(R0);
	define("I", hold(QI));
	define("void", hold(Qvoid));
	define(".", E(E(new_exp(&type_concat))));
	define("say", E(new_exp(&type_say)));
	define("put", E(new_exp(&type_put)));
	define("nl", new_exp(&type_nl));
	define("num_str", E(new_exp(&type_num_str)));

	define("+", E(E(new_exp(&type_add))));
	define("-", E(E(new_exp(&type_sub))));
	define("*", E(E(new_exp(&type_mul))));
	define("/", E(E(new_exp(&type_div))));
	define("xor", E(E(new_exp(&type_xor))));
	}

static void end_std(void)
	{
	drop(cx_std);
	}

int main(int _argc, const char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	init_signal();

	limit_time(1); // LATER Perhaps use alarm for sub-second limits.
	limit_stack(20000);
	limit_memory(10000000);

	beg_basic();
	beg_std();

	run_script(cx_std);

	end_basic();
	end_std();

	free_memory(alt_stack.ss_sp, alt_stack.ss_size);
	clear_free_list();
	end_memory();
	return 0;
	}
