#include <value.h>
#include <basic.h>
#include <die.h>
#include <num.h>
#include <output.h>
#include <stdio.h>
#include <str.h>
#include <sys/file.h> /* flock */
#include <type_file.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

value type_file(value f)
	{
	return type_void(f);
	}

static void file_free(FILE *fh)
	{
	if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
		fclose(fh);
	}

value Qfile(FILE *fh)
	{
	return D(type_file,fh,(type)file_free);
	}

/* (fopen path mode) Open a file and return {fh}, or void if not possible. */
value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	if (x->T == type_str && y->T == type_str)
		{
		string path = data(x);
		string mode = data(y);
		FILE *fh = fopen(path->data,mode->data);
		if (fh)
			f = A(Q(type_single),Qfile(fh));
		else
			f = Q(type_void);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (remove path) = {code} Remove path from file system.  The code is 0 if
successful or -1 otherwise. */
value type_remove(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_str)
		{
		string path = data(x);
		int code = remove(path->data);
		f = A(Q(type_single), Qnum0(code));
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}

static value op_flock(value f, int operation)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		int code = flock(fileno(fh),operation);
		if (code < 0)
			{
			perror("flock");
			die(0);
			}
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}

/* (flock_ex fh) Obtain an exclusive lock on the file handle, blocking as long
as necessary. */
value type_flock_ex(value f) { return op_flock(f,LOCK_EX); }

/* (flock_sh fh) Obtain a shared lock on the file handle, blocking as long
as necessary. */
value type_flock_sh(value f) { return op_flock(f,LOCK_SH); }

/* (flock_un fh) Unlock the file handle. */
value type_flock_un(value f) { return op_flock(f,LOCK_UN); }

/* LATER strerror */
/* LATER fdopen fflush */
/* LATER pipe dup2 close fork wait */