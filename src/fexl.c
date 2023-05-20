#include <die.h>
#include <limit.h>
#include <memory.h>
#include <signal.h> // sigaction
#include <stdio.h>

#include <value.h>
#include <app.h>
#include <lam.h>
#include <ref.h>

#include <str.h>
#include <buffer.h>
#include <stream.h>

#include <parse.h>
#include <type_str.h>

#include <show.h>

static FILE *cur_fh;

static int get(void)
	{
	//return '('; // Test stack overflow in parser.
	return fgetc(cur_fh);
	}

value parse_script(const char *name)
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

	exp = parse_fexl();

	fclose(cur_fh);
	cur_fh = 0;
	return exp;
	}

int argc;
const char **argv;

// Benchmark version of eval which counts reduction steps.

static unsigned long num_steps = 0;

static value eval_count(value pair)
	{
	while (1)
		{
		value next = pair->L->type->step(pair);
		if (next == 0) break;
		num_steps++;
		drop(pair);
		pair = next;
		}
	return pair;
	}

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

static void run_script(void)
	{
	init_signal();

	limit_time(1); // LATER Perhaps use alarm for sub-second limits.
	limit_stack(20000);
	limit_memory(10000000);

	{
	const char *name = argc > 1 ? argv[1] : "";
	value exp = parse_script(name);
	show_line("exp = ",exp);
	clear_free_list();

	{
	value pair = A(exp,R(0));
	unsigned long old_bytes = cur_bytes;

	eval = eval_count;
	pair = eval(pair);
	show_line("pair = ",pair);
	{
	unsigned long num_bytes = cur_bytes - old_bytes;
	printf("num_steps = %lu\n",num_steps);
	printf("num_bytes = %lu\n\n",num_bytes);
	}
	drop(pair);
	}
	}
	}

int main(int _argc, const char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	run_script();

	free_memory(alt_stack.ss_sp, alt_stack.ss_size);
	clear_free_list();
	end_memory();
	return 0;
	}
