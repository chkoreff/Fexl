#include <value.h>

#include <basic.h>
#include <define.h>
#include <signal.h> // sigaction etc.
#include <stdio.h> // perror
#include <stdlib.h> // exit
#include <type_num.h>
#include <type_signal.h>
#include <unistd.h> // alarm

static void handle_signal(int signum)
	{
	(void)signum;
	}

// Reference:
// https://stackoverflow.com/questions/31784823/interrupting-open-with-sigalrm
//
// This sets a signal handler so it does not kill the process when the signal
// happens, but instead interrupts any system call in progress such as fget or
// fread.
static void set_handler(int signum)
	{
	struct sigaction sigact;
	sigact.sa_handler = handle_signal;
	sigact.sa_flags = 0; // Override default SA_RESTART.
	sigemptyset(&sigact.sa_mask);
	if (sigaction(signum, &sigact, NULL) < 0)
		{
		perror("sigaction(2) error");
		exit(1);
		}
	}

// (set_alarm seconds)
// Set an alarm for the given number of seconds.  This is used primarily by
// server processes so they don't wait forever for input from the client.  If
// the alarm goes off it will interrupt the system call (fget, fread, etc.) and
// the process can exit.
static value type_set_alarm(value f)
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		alarm(get_ulong(x));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

void define_signal(void)
	{
	define("set_alarm",Q(type_set_alarm));
	}

void beg_signal(void)
	{
	set_handler(SIGALRM);
	set_handler(SIGCHLD);
	}

void end_signal(void)
	{
	}
