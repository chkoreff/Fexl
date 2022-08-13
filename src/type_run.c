#include <str.h>
#include <value.h>

#include <arpa/inet.h> /* inet_addr */
#include <basic.h>
#include <die.h>
#include <memory.h>
#include <netinet/in.h> /* IPPROTO_TCP INADDR_ANY (BSD) */
#include <signal.h> /* kill (BSD) */
#include <stdio.h>
#include <stdlib.h> /* exit */
#include <sys/socket.h>
#include <sys/types.h> /* pid_t */
#include <sys/wait.h> /* wait */
#include <termios.h> /* tcgetattr etc. */
#include <type_file.h>
#include <type_num.h>
#include <type_run.h>
#include <type_str.h>
#include <unistd.h> /* exec fork sleep */

int main_argc;
const char **main_argv;

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

static value op_sleep(value f, unsigned int op(unsigned int))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		unsigned int n = get_ulong(x);
		op(n);
		f = hold(QI);
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
	return op_sleep(f,sleep);
	}

/* (usleep n) Sleep for the specified number of microseconds. */
value type_usleep(value f)
	{
	return op_sleep(f,(unsigned int (*)(unsigned int))usleep);
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

/* Interact with the fn_child function as a separate process.

If catch_stderr is true, evaluate:
	(fn_parent child_in child_out child_err)

If catch_stderr is false, evaluate:
	(fn_parent child_in child_out)

The child_in, child_out, and child_err are file handles for the child's stdin,
stdout, and stderr respectively.

That evaluation performs an interaction using the file handles, returning a
handler function which receives the child's exit status when the child process
terminates.
*/
static value op_process(value f, int catch_stderr)
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
	if (catch_stderr)
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
		if (catch_stderr)
			{
			/* Duplicate write side of error pipe to stderr. */
			do_dup2(fd_err[1],2);
			}

		/* Close unused file handles.  They're actually all unused because I
		duplicated the ones I still need.  At a minimum, I must close the write
		side of the input pipe, otherwise the child hangs waiting for stdin to
		close. */

		do_close(fd_in[0]);
		do_close(fd_in[1]); /* Must do this one to avoid hang. */

		do_close(fd_out[0]);
		do_close(fd_out[1]);

		if (catch_stderr)
			{
			do_close(fd_err[0]);
			do_close(fd_err[1]);
			}

		/* Evaluate the child function. */
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
		FILE *child_err = 0;
		if (catch_stderr)
			{
			/* Open read side of error pipe as child error. */
			child_err = do_fdopen(fd_err[0],"r");
			}

		/* Close unused file handles.  I don't close the ones I just opened
		because they are still in play (i.e. fdopen does not dup). */

		do_close(fd_in[0]);  /* Close the read side of the input pipe. */
		do_close(fd_out[1]); /* Close the write side of the output pipe. */
		if (catch_stderr)
			do_close(fd_err[1]); /* Close the write side of the error pipe. */

		{
		value handler = A(A(hold(f->R),Qfile(child_in)),Qfile(child_out));
		int status;
		if (catch_stderr)
			handler = A(handler,Qfile(child_err));

		handler = eval(handler);
		status = do_wait(pid);
		return A(handler,Qnum(status));
		}
		}
	}
	}
	}
	}

/* (run_process fn_child fn_parent)

Interact with the fn_child function as a separate process, with the fn_parent
receiving handles to the child's stdin and stdout.

The child's stderr goes to the same destination as the parent's stderr, which
is typically what you want when implementing a server with an error log.
*/
value type_run_process(value f)
	{
	return op_process(f,0);
	}

/* (spawn fn_child fn_parent)

Interact with the fn_child function as a separate process, with the fn_parent
receiving handles to the child's stdin, stdout, and stderr.
*/
value type_spawn(value f)
	{
	return op_process(f,1);
	}

static void die_perror(const char *msg)
	{
	perror(msg);
	die(0);
	}

static int make_socket(const char *ip, unsigned long port)
	{
	int fd_listen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd_listen == -1)
		die_perror("socket");

	/* Set flags so you can restart the server quickly.  Otherwise you get
	the error "Address already in use" if you stop the server while a
	client is connected and then try to restart it within TIME_WAIT
	interval.  */
	{
	int on = 1;
	setsockopt(fd_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
	}

	/* Bind the socket to the ip and port. */
	{
	unsigned int count = 10; /* Max retries, usually 1 suffices. */
	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);

	while (1)
		{
		if (bind(fd_listen, (struct sockaddr *)&addr, sizeof(addr)) != -1)
			break;

		if (count == 0)
			die_perror("bind");

		/* If the previous server process has been killed but the socket system
		hasn't quite released the address yet, the bind can fail.  In that case
		I try again to give it a little more time. */

		count--;
		usleep(10); /* Sleep 10 microseconds. */
		}
	}

	/* Listen to the socket. */
	{
	int backlog = 10;
	if (listen(fd_listen, backlog) == -1)
		die_perror("listen");
	}

	return fd_listen;
	}

static void server_process(value v_interact, int fd_listen)
	{
	while (1)
		{
		/* If a child exits I get a SIGCHLD signal which interrupts
		any system call, giving me a chance to wait for any child
		processes to finish here.  This prevents defunct processes. */
		while (1)
			{
			int status;
			int pid = waitpid(-1,&status,WNOHANG);
			if (pid <= 0) break;
			}

		/* Accept an inbound connection. */
		{
		struct sockaddr_in addr;
		socklen_t size = sizeof(addr);

		int fd_remote = accept(fd_listen, (struct sockaddr *)&addr, &size);
		if (fd_remote == -1)
			continue;

		/* Fork a process to handle the connection. */
		{
		pid_t pid = fork();
		if (pid == -1) die("fork failed");
		if (pid == 0)
			{
			/* This is the child process.  Close the existing stdin and stdout
			and replace them with the client socket. */

			/* Close the listening socket so you can restart the server after
			killing it with a client still connected.  Any such clients
			continue to run with the old server process, but the new server
			process can now receive connections. */
			close(fd_listen);

			close(0);
			close(1);
			/* Duplicate client socket pipe to stdin. */
			do_dup2(fd_remote,0);
			/* Duplicate client socket pipe to stdout. */
			do_dup2(fd_remote,1);

			/* Make stdout unbuffered to you don't have to call fflush. */
			setvbuf(stdout,0,_IONBF,0);

			do_close(fd_remote);

			/* Interact with the client. */
			drop(eval(hold(v_interact)));

			exit(0);
			}
		else
			{
			/* This is the parent process.  Close the client socket and
			loop back around to accept the next inbound connection. */
			do_close(fd_remote);
			}
		}
		}
		}
	}

/* (start_server ip port error_log interact)

Start a server process in the background which handles inbound connections to
the ip address and port.  Any error output from the server is redirected into
the error_log file, unless error_log is "" in which case it goes to stderr.

After starting the server process, this function returns immediately to the
caller.  Meanwhile the server process loops forever listening for connections.
When a connection occurs, it runs the given interact function with stdin and
stdout set up to communicate with the client connection like an ordinary
filter.

NOTE Handy command:
lsof -i:2186 -t
Lists all pids either listening or connected to port 2186.
Also:
lsof -i4TCP@127.0.0.1:2186 -t
*/
value type_start_server(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	{
	value v_ip = arg(f->L->L->L->R);
	value v_port = arg(f->L->L->R);
	value v_error_log = arg(f->L->R);
	value v_interact = hold(f->R);

	if (v_ip->T == type_str &&
		v_port->T == type_num &&
		v_error_log->T == type_str)
		{
		const char *ip = str_data(v_ip);
		unsigned long port = get_ulong(v_port);
		const char *name_error_log = str_data(v_error_log);
		FILE *fh_log;

		int fd_listen = make_socket(ip,port);

		if (name_error_log[0] == '\000')
			fh_log = 0;
		else
			{
			fh_log = fopen(name_error_log,"a+");
			if (fh_log == 0)
				die("Could not open error log.");
			}

		{
		/* Fork the server so it runs in the background. */
		pid_t pid = fork();
		if (pid == -1) die("fork failed");
		if (pid == 0)
			{
			/* Child process: run the server. */
			/* Make stderr go to the log file. */
			if (fh_log)
				{
				do_dup2(fileno(fh_log),2);
				fclose(fh_log);
				}
			server_process(v_interact,fd_listen); /* Never returns. */
			}
		else
			{
			/* Parent process: close any unused file handles. */
			if (fh_log)
				fclose(fh_log);
			close(fd_listen);
			f = hold(QI);
			}
		}
		}
	else
		f = hold(Qvoid);

	drop(v_ip);
	drop(v_port);
	drop(v_error_log);
	drop(v_interact);
	return f;
	}
	}

/* (kill pid sig) */
value type_kill(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value v_pid = arg(f->L->R);
	value v_sig = arg(f->R);
	if (v_pid->T == type_num && v_sig->T == type_num)
		{
		int pid = get_double(v_pid);
		int sig = get_double(v_sig);
		(void)kill(pid,sig);
		{
		int status;
		pid_t result = waitpid(pid,&status,0);
		(void)result;
		}
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(v_pid);
	drop(v_sig);
	return f;
	}
	}

/* LATER 20181030 Interface to DNS (gethostbyname) */

/* (connect ip port) Connect to the ip address and port and return the file
handle for the connection. */
value type_connect(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		int fd_remote = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (fd_remote == -1)
			f = hold(Qvoid);
		else
			{
			const char *ip = str_data(x);
			unsigned long port = get_ulong(y);
			struct sockaddr_in addr;
			int retval;

			addr.sin_family = PF_INET;
			addr.sin_addr.s_addr = inet_addr(ip);
			addr.sin_port = htons(port);
			retval = connect(fd_remote, (struct sockaddr *)&addr, sizeof(addr));

			if (retval == -1)
				f = hold(Qvoid);
			else
				f = Qfile(do_fdopen(fd_remote,"r+"));
			}
		}
	else
		f = hold(Qvoid);

	drop(x);
	drop(y);
	return f;
	}
	}

/* (exec argv) Call execv with the given argument list.  The first argument is
the full path of the executable program.  This call does not return. */
value type_exec(value f)
	{
	if (!f->L) return 0;
	{
	struct collect collect;
	beg_collect(f->R,&collect,"bad arg to exec");

	fflush(stdout); /* Flush any existing output. */

	{
	/* Call execv with the argument list. */
	char *const *argv = (void *)collect.argv;
	if (execv(argv[0],argv) == -1)
		die("exec failed");
	}

	/* Although execv doesn't return, here is the code to clean up memory. */
	end_collect(&collect);

	return hold(QI);
	}
	}

/* (receive_keystrokes fn)
Run the function while receiving individual keystrokes. */
value type_receive_keystrokes(value f)
	{
	if (!f->L) return 0;
	{
	struct termios attr;
	tcflag_t save_c_lflag;

	/* Set terminal to receive individual keystrokes. */
	tcgetattr(0, &attr);
	save_c_lflag = attr.c_lflag; /* Save original local modes. */

	attr.c_lflag &= ~ICANON;
	attr.c_lflag &= ~ECHO;
	tcsetattr(0, TCSANOW, &attr);

	f = arg(f->R);

	/* Restore original local modes. */
	attr.c_lflag = save_c_lflag;
	tcsetattr(0, TCSANOW, &attr);

	return f;
	}
	}

static unsigned long num_steps = 0;

/* (fexl_benchmark x next) Evaluate x and return (next val steps bytes), where
val is the value of x, steps is the number of reduction steps, and bytes is the
number of memory bytes used. */
static value eval_count(value f)
	{
	while (1)
		{
		value g = f->T(f);
		if (g == 0) return f;
		num_steps++;
		drop(f);
		f = g;
		}
	}

value type_fexl_benchmark(value f)
	{
	if (!f->L || !f->L->L) return 0;
	clear_free_list();
	{
	value (*save_eval)(value f) = eval;
	unsigned long save_num_steps = num_steps;
	unsigned long save_cur_bytes = cur_bytes;

	eval = eval_count;
	num_steps = 0;

	{
	value x = arg(f->L->R);
	double steps = (double)num_steps;
	double bytes = (double)cur_bytes - (double)save_cur_bytes;
	f = A(A(A(hold(f->R),x),Qnum(steps)),Qnum(bytes));
	}

	eval = save_eval;
	num_steps += save_num_steps;
	return f;
	}
	}
