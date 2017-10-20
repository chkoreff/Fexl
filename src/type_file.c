#include <str.h>
#include <sys/types.h>

#include <input.h>
#include <num.h>
#include <stdio.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <memory.h>
#include <standard.h>
#include <sys/file.h> /* flock */
#include <sys/stat.h> /* stat */
#include <type_file.h>
#include <type_input.h>
#include <type_num.h>
#include <type_str.h>
#include <unistd.h> /* readlink */

value type_file(value f)
	{
	if (f->N == 0)
		return 0;
	return type_void(f);
	}

value Qfile(FILE *fh)
	{
	return D(type_file,fh);
	}

FILE *get_fh(value x)
	{
	return (FILE *)x->R;
	}

/* (fopen path mode) Open a file and return fh, where fh is the open file
handle or void on failure. */
value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		{
		const char *path = str_data(x);
		const char *mode = str_data(y);
		FILE *fh = fopen(path,mode);
		f = fh ? Qfile(fh) : hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (fclose fh) Close the file handle, and set it to void so it doesn't core
dump if you mistakenly try to close it again. */
value type_fclose(value f)
	{
	if (!f->L) return 0;
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		fclose(get_fh(out));
		out->T = type_void;
		out->R = 0;
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}

/* (fgetc fh) returns the next single byte from the file, or void if none. */
value type_fgetc(value f)
	{
	return op_getc(f,type_file,(input)fgetc);
	}

/* (fget in) returns the next UTF-8 character from the file, or void if none. */
value type_fget(value f)
	{
	return op_get(f,type_file,(input)fgetc);
	}

/* (remove path) Remove path from file system; return 0 if successful or -1
otherwise. */
value type_remove(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *path = str_data(x);
		int code = remove(path);
		f = Qnum0(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Return true if file1 is newer than file2.  If either file is missing, return
true.  That makes the most sense for caching operations.

NOTE: Formerly I included the nanosecond resolution using the following code,
but that doesn't compile on older machines so I'm sticking with whole second
resolution.

	if (status_1.st_mtim.tv_sec > status_2.st_mtim.tv_sec)
		return 1;

	if (status_1.st_mtim.tv_sec == status_2.st_mtim.tv_sec &&
		status_1.st_mtim.tv_nsec > status_2.st_mtim.tv_nsec)
		return 1;
*/
static int is_newer(const char *file1, const char *file2)
	{
	int ret;
	struct stat status_1;
	struct stat status_2;

	ret = stat(file1,&status_1);
	if (ret != 0) return 1;

	ret = stat(file2,&status_2);
	if (ret != 0) return 1;

	return status_1.st_mtime > status_2.st_mtime;
	}

value type_is_newer(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = boolean(is_newer(str_data(x),str_data(y)));
	else
		f = hold(Qvoid);

	drop(x);
	drop(y);
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
		int code = flock(fileno(get_fh(x)),operation);
		if (code < 0)
			{
			perror("flock");
			die(0);
			}
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
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

/* LATER fdopen pipe dup2 close fork wait alarm */

/* Call readlink, returning a string. */
static string safe_readlink(const char *path)
	{
	char *buf;
	long len;
	long size = 256;

	while (1)
		{
		buf = new_memory(size);
		len = readlink(path, buf, size);

		if (len == size)
			{
			/* Used all available space, so the result might be truncated. */
			free_memory(buf, size);
			size = 2 * size;
			}
		else
			{
			/* Used less than available space, so the result fits just fine.
			A system error yields len == -1, but that works robustly. */
			string result = str_new_data(buf,len);
			free_memory(buf, size);
			return result;
			}
		}
	}

value type_readlink(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qstr(safe_readlink(str_data(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* \code=(mkdir path mode) Attempt to create a directory named path.
See mkdir(2) for details. */
value type_mkdir(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		const char *path = str_data(x);
		unsigned int mode = get_double(y);
		int code = mkdir(path,mode);
		f = Qnum0(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* \code=(rmdir path) Deletes a directory, which must be empty.
See rmdir(2) for details. */
value type_rmdir(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *path = str_data(x);
		int code = rmdir(path);
		f = Qnum0(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
