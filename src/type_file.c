#include <str.h>
#include <value.h>
#include <basic.h>
#include <die.h>
#include <input.h>
#include <num.h>
#include <stdio.h>
#include <sys/file.h> /* flock */
#include <type_file.h>
#include <type_input.h>
#include <type_num.h>
#include <type_str.h>

static void file_free(FILE *fh)
	{
	if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
		fclose(fh);
	}

value type_file(value f)
	{
	if (f->N == 0)
		{
		file_free((FILE *)f->R);
		return 0;
		}
	return type_void(f);
	}

value Qfile(FILE *fh)
	{
	return D(type_file,fh);
	}

/* (fopen path mode) Open a file and yield fh, where fh is the open file
handle or void on failure. */
value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		{
		string path = (string)x->R;
		string mode = (string)y->R;
		FILE *fh = fopen(path->data,mode->data);
		action = 1;
		f = yield(fh ? Qfile(fh) : Qvoid());
		}
	else
		reduce_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (fgetc fh) yields the next single byte from the file, or void if no more. */
value type_fgetc(value f)
	{
	return op_getc(f,type_file,(input)fgetc);
	}

/* (fget in) yields the next UTF-8 character from the file, or void if no more.
*/
value type_fget(value f)
	{
	return op_get(f,type_file,(input)fgetc);
	}

/* (remove path) Remove path from file system; yield 0 if successful or -1
otherwise. */
value type_remove(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		string path = (string)x->R;
		int code = remove(path->data);
		action = 1;
		f = yield(Qnum0(code));
		}
	else
		reduce_void(f);
	drop(x);
	return f;
	}
	}

static value op_flock(value f, int operation)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = (FILE *)x->R;
		int code = flock(fileno(fh),operation);
		if (code < 0)
			{
			perror("flock");
			die(0);
			}
		action = 1;
		f = QI();
		}
	else
		reduce_void(f);
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
/* LATER fdopen */
/* LATER pipe dup2 close fork wait */
