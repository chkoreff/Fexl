#include <stdio.h>
#include <str.h>
#include <value.h>

#include <context.h>
#include <die.h>
#include <memory.h>
#include <parse.h>
#include <signal.h> // sigaction
#include <type_str.h>

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

static void beg_signal(void)
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

static void end_signal(void)
	{
	free_memory(alt_stack.ss_sp, alt_stack.ss_size);
	}

int main(int argc, const char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	FILE *fh = open_source(name);

	beg_signal();
	beg_context();

	drop(eval(load(Qstr0(name),fh)));

	end_context();
	end_signal();

	end_memory();
	return 0;
	}
