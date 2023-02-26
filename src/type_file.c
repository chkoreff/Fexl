#include <str.h>
#include <sys/types.h>

#include <input.h>
#include <stdio.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <dirent.h> /* opendir readdir closedir */
#include <memory.h>
#include <sys/file.h> /* flock */
#include <sys/stat.h> /* stat */
#include <type_file.h>
#include <type_input.h>
#include <type_num.h>
#include <type_str.h>
#include <unistd.h> /* readlink */

value Qstdin;
value Qstdout;
value Qstderr;

value type_file(value f)
	{
	return type_atom(f);
	}

/* Automatically close the file handle unless it's stdin, stdout, or stderr.
That way I can still see any error message at the very end of the program.
*/
static void fh_free(FILE *fh)
	{
	if (fileno(fh) > 2)
		fclose(fh);
	}

value Qfile(FILE *fh)
	{
	static struct value atom = {0, (type)fh_free};
	return V(type_file,&atom,(value)fh);
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
		out->L = 0;
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

/* (fget fh) returns the next UTF-8 character from the file, or void if none. */
value type_fget(value f)
	{
	return op_get(f,type_file,(input)fgetc);
	}

value type_clearerr(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = get_fh(x);
		clearerr(fh);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

value type_feof(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = get_fh(x);
		f = boolean(feof(fh));
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* (flook fh) returns the next byte from the file without consuming it. */
value type_flook(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = get_fh(x);
		int ch = fgetc(fh);
		if (ch == -1)
			f = hold(Qvoid);
		else
			{
			char c = (char)ch;
			(void)ungetc(ch,fh);
			f = Qstr(str_new_data(&c,1));
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
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
		f = Qnum(code);
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

static value op_stat_type(value f, mode_t mask)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *path = str_data(x);
		struct stat status;
		int result = stat(path,&status);
		if (result == -1)
			result = 0;
		else
			result = ((status.st_mode & S_IFMT) == mask);
		f = boolean(result);
		}
	else
		f = hold(Qvoid);

	drop(x);
	return f;
	}
	}

/* (is_file path) Return true if the path is a regular file. */
value type_is_file(value f)
	{
	return op_stat_type(f,S_IFREG);
	}

/* (is_dir path) Return true if the path is a directory. */
value type_is_dir(value f)
	{
	return op_stat_type(f,S_IFDIR);
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
			Return null string on system error (len == -1). */
			string result = str_new_data(buf,(len < 0 ? 0 : len));
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
See mkdir(2) for details.  An example for mode is (oct "775"). */
value type_mkdir(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		const char *path = str_data(x);
		unsigned long mode = get_ulong(y);
		int code = mkdir(path,mode);
		f = Qnum(code);
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
		f = Qnum(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* \code=(ftruncate fh len) Truncate a file to the given length. */
value type_ftruncate(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_file && y->T == type_num)
		{
		FILE *fh = get_fh(x);
		unsigned long len = get_ulong(y);
		int code = ftruncate(fileno(fh),len);
		f = Qnum(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value op_seek(value f, int whence)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_file && y->T == type_num)
		{
		FILE *fh = get_fh(x);
		long offset = get_double(y);
		int code = fseek(fh,offset,whence);
		f = Qnum(code);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* \code=(fseek_set fh offset) */
value type_fseek_set(value f) { return op_seek(f,SEEK_SET); }
value type_fseek_cur(value f) { return op_seek(f,SEEK_CUR); }
value type_fseek_end(value f) { return op_seek(f,SEEK_END); }

/* \offset=(ftell fh) */
value type_ftell(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = get_fh(x);
		long offset = ftell(fh);
		f = Qnum(offset);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* \n=(fileno fh) */
value type_fileno(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = get_fh(x);
		int n = fileno(fh);
		f = Qnum(n);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* \str=(fread fh size) Read at most size bytes from the file, clipping if it
reaches end of file. */
value type_fread(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_file && y->T == type_num)
		{
		FILE *fh = get_fh(x);
		unsigned long size = get_ulong(y);
		string str = str_new(size);
		size_t count = fread(str->data,1,size,fh);
		if (count < size)
			{
			string next = str_new_data(str->data,count);
			str_free(str);
			str = next;
			}
		f = Qstr(str);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* \fh=(mkfile path mode) Atomically create and open a file for reading and
writing.  Returns void if the file already existed. */
value type_mkfile(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_num)
		{
		const char *path = str_data(x);
		unsigned int mode = get_ulong(y);
		int fd = open(path, O_CREAT|O_RDWR|O_EXCL, mode);
		FILE *fh = (fd != -1) ? fdopen(fd,"r+") : 0;
		f = fh ? Qfile(fh) : hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value dir_names(DIR *dir)
	{
	struct dirent *entry = readdir(dir);
	if (!entry) return hold(Qnull);
	return V(0,Qstr0(entry->d_name),dir_names(dir));
	}

/* \names=(dir_names path) Return the list of names in a directory.  The names
are returned in arbitrary order and include the special files "." and "..". */
value type_dir_names(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *path = str_data(x);
		DIR *dir = opendir(path);
		if (dir)
			{
			f =  AV(hold(Qlist),dir_names(dir));
			closedir(dir);
			}
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

static unsigned long get_st_mtime(struct stat *status)
	{ return status->st_mtime; }
static unsigned long get_st_size(struct stat *status)
	{ return status->st_size; }

static value op_stat(value f, unsigned long op(struct stat *status))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *path = str_data(x);
		struct stat status;
		int result = stat(path,&status);
		if (result == -1)
			f = hold(Qvoid);
		else
			{
			unsigned long n = op(&status);
			f = Qnum(n);
			}
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

/* Return the modification time of a file in epoch seconds. */
value type_mod_time(value f)
	{
	return op_stat(f,get_st_mtime);
	}

/* Return the size of a file. */
value type_file_size(value f)
	{
	return op_stat(f,get_st_size);
	}

/* (symlink target linkpath) Create a symbolic link named linkpath which points
to target.  Return the numeric result of calling symlink(2). */
value type_symlink(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = Qnum(symlink(str_data(x),str_data(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (rename source target) Rename source path as target path.  Return the
numeric value of calling rename(2). */
value type_rename(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_str && y->T == type_str)
		f = Qnum(rename(str_data(x),str_data(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

void beg_file(void)
	{
	Qstdin = Qfile(stdin);
	Qstdout = Qfile(stdout);
	Qstderr = Qfile(stderr);
	}

void end_file(void)
	{
	drop(Qstdin);
	drop(Qstdout);
	drop(Qstderr);
	}
