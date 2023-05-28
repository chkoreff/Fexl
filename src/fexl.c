#include <stdio.h>
#include <value.h>

#include <context.h>
#include <die.h>
#include <memory.h>
#include <parse.h>
#include <show.h>
#include <signal.h> // sigaction

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

// Set a handler to catch a signal instead of killing the process.
static void set_handler(int signum)
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

static void init_signal(void)
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

static unsigned long num_steps;

static value count_step_A(value f)
	{
	num_steps++;
	return step_A(f);
	}

int main(int argc, const char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	FILE *fh = name[0] ? fopen(name,"r") : stdin;
	if (!fh)
		{
		fprintf(stderr,"Could not open source file %s\n",name);
		die(0);
		}

	init_signal();

	beg_std();

	{
	value exp = parse_fexl_fh(name,fh,cx_std);
	show_line("BEG exp = ",exp);

	type_A.step = count_step_A;
	exp = eval(exp);
	type_A.step = step_A;

	show_line("END exp = ",exp);
	printf("steps %lu bytes %lu\n",num_steps,cur_bytes);
	drop(exp);
	}

	end_std();

	free_memory(alt_stack.ss_sp, alt_stack.ss_size);
	clear_free_list();
	end_memory();
	return 0;
	}
