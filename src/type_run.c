#include <num.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <standard.h>
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait */
#include <type_file.h>
#include <type_num.h>
#include <type_run.h>
#include <type_str.h>
#include <unistd.h> /* exec fork sleep */

int main_argc;
char **main_argv;

value type_die(value f)
	{
	die(0);
	return f;
	}

/* (argv i) Return the command line argument at position i (starting at 0), or
void if no such position. */
value type_argv(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		int i = get_double(x);
		if (i >= 0 && i < main_argc)
			f = Qstr(str_new_data0(main_argv[i]));
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (sleep n) Sleep for the specified number of seconds. */
value type_sleep(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		unsigned int n = get_ulong(x);
		sleep(n);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static void do_pipe(int fd[])
	{
	int status = pipe(fd);
	if (status == -1) die("pipe failed");
	}

static void do_dup2(int oldfd, int newfd)
	{
	int status = dup2(oldfd,newfd);
	if (status == -1) die("dup2 failed");
	}

static void do_close(int fd)
	{
	int status = close(fd);
	if (status == -1) die("close failed");
	}

static FILE *do_fdopen(int fd, const char *mode)
	{
	FILE *fh = fdopen(fd,mode);
	if (fh == 0) die("fdopen failed");
	return fh;
	}

static int do_wait(pid_t pid)
	{
	int status;
	pid_t result = waitpid(pid,&status,0);
	if (result == -1) die("waitpid failed");
	return status;
	}

/* \result=(spawn fn_child fn_parent) Spawn the child function as a separate
process.  Pass handles to the child's stdin, stdout, and stderr to the parent
function and evaluate that.  Then wait for the child to finish, and pass the
child's exit code to that.  Return the result.
*/
value type_spawn(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	/* Flush the parent's stdout and stderr to prevent any pending output from
	being accidentally pushed into the child's input.  I've noticed this can
	happen when the script output is sent to a file or pipe instead of a
	console.
	*/
	fflush(stdout);
	fflush(stderr);

	{
	/* Create a series of pipes, each with a read and write side. */
	int fd_in[2];
	int fd_out[2];
	int fd_err[2];

	do_pipe(fd_in);
	do_pipe(fd_out);
	do_pipe(fd_err);

	{
	pid_t pid = fork();
	if (pid == -1) die("fork failed");

	if (pid == 0)
		{
		/* This is the child process. */

		/* Duplicate read side of input pipe to stdin. */
		do_dup2(fd_in[0],0);
		/* Duplicate write side of output pipe to stdout. */
		do_dup2(fd_out[1],1);
		/* Duplicate write side of error pipe to stderr. */
		do_dup2(fd_err[1],2);

		/* Close unused file handles.  They're actually all unused because I
		duplicated the ones I still need.  At a minimum, I must close the write
		side of the input pipe, otherwise the child hangs waiting for stdin to
		close. */

		do_close(fd_in[0]);
		do_close(fd_in[1]); /* Must do this one to avoid hang. */

		do_close(fd_out[0]);
		do_close(fd_out[1]);

		do_close(fd_err[0]);
		do_close(fd_err[1]);

		/* Evaluate the child function, which can now use stdin, stdout, and
		stderr normally. */

		drop(eval(hold(f->L->R)));

		/* Exit here to avoid continuing with evaluation.  This means that
		memory leak detection does not occur for the child function.  If you
		want that level of checking, you should exec with (argv 0) instead. */
		exit(0);
		return 0;
		}
	else
		{
		/* This is the parent process. */

		/* Open write side of input pipe as child input. */
		FILE *child_in = do_fdopen(fd_in[1],"w");
		/* Open read side of output pipe as child output. */
		FILE *child_out = do_fdopen(fd_out[0],"r");
		/* Open read side of error pipe as child error. */
		FILE *child_err = do_fdopen(fd_err[0],"r");

		/* Close unused file handles.  I don't close the ones I just opened
		because they are still in play (i.e. fdopen does not dup). */

		do_close(fd_in[0]);  /* Close the read side of the input pipe. */
		do_close(fd_out[1]); /* Close the write side of the output pipe. */
		do_close(fd_err[1]); /* Close the write side of the error pipe. */

		{
		value result = eval(A(A(A(hold(f->R),Qfile(child_in)),
			Qfile(child_out)),Qfile(child_err)));
		int status = do_wait(pid);
		result = eval(A(result,Qnum0(status)));
		return result;
		}
		}
	}
	}
	}
	}

static void do_execv(const char **argv)
	{
	const char *path = argv[0];
	int result = execv(path,(char *const *)argv);
	if (result == -1) die("exec failed");
	}

/* (exec argv) Call execv with the given argument list.  The first argument is
the full path of the executable program.  This call does not return. */
value type_exec(value f)
	{
	if (!f->L) return 0;
	{
	/* Gather args onto a stack in reverse order. */
	value stack = hold(Qnull);
	value list = hold(f->R);
	unsigned long len = 0;
	while (1)
		{
		list = eval(list);
		if (list->T == type_cons && list->L && list->L->L)
			{
			value item = arg(list->L->R);
			value tail = hold(list->R);
			if (item->T != type_str)
				die("bad arg to exec");
			stack = A(stack,item);
			len++;
			drop(list);
			list = tail;
			}
		else if (list->T == type_null)
			break;
		else
			die("bad arg to exec");
		}
	drop(list);

	{
	/* Allocate an array of pointers and store the stack entries there. */
	unsigned long size = (len+1) * sizeof(char *);
	const char **argv = new_memory(size);
	value curr = stack;

	argv[len] = 0; /* ending sentinel */
	while (curr->T == type_A)
		{
		value next = hold(curr->L);
		value item = curr->R;

		len--;
		argv[len] = str_data(item);

		curr = next;
		}

	/* Call execv with the argument list. */
	do_execv(argv);

	/* Although execv doesn't return, here is the code to clean up memory. */
	free_memory(argv,size);
	drop(stack);
	}

	return hold(QI);
	}
	}

static unsigned long num_steps = 0;

static value step_count(value f)
	{
	num_steps++;
	return f->T(f);
	}

/* (fexl_benchmark x next) Evaluate x and return (next val steps bytes), where
val is the value of x, steps is the number of reduction steps, and bytes is the
number of additional memory bytes used. */
value type_fexl_benchmark(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value (*save_step)(value) = step;
	unsigned long save_num_steps = num_steps;
	unsigned long save_cur_bytes = cur_bytes;
	step = step_count;

	{
	value x = arg(f->L->R);
	double steps = (double)num_steps - (double)save_num_steps;
	double bytes = (double)cur_bytes - (double)save_cur_bytes;
	f = A(A(A(hold(f->R),x),Qnum0(steps)),Qnum0(bytes));
	}

	step = save_step;
	if (step != step_count)
		num_steps = save_num_steps;
	return f;
	}
	}
