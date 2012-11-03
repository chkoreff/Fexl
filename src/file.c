#include <stdio.h>
#include <unistd.h> /* readlink */
#include "buf.h"
#include "value.h"
#include "basic.h"
#include "file.h"
#include "long.h"
#include "memory.h"
#include "string.h"

void type_file(value f) { type_error(); }

FILE *get_file(value f)
	{
	return (FILE *)f->R->L;
	}

/* Close the file when it's no longer used. */
static void clear_file(value f)
	{
	FILE *fh = get_file(f);
	if (fh) fclose(fh);
	}

/* Make a file value from the given file handle. */
value Qfile(FILE *fh)
	{
	value f = D(type_file);
	f->R->T = clear_file;
	f->R->L = (value)fh;
	return f;
	}

/* Make a file value from a constant file handle which should not be
automatically closed, such as stdout. */
value Qfile_const(FILE *fh)
	{
	value f = Qfile(fh);
	f->R->T = 0; /* Do not close. */
	return f;
	}

/* (fopen path mode) opens the file with the given path and returns:
  yes fh   # if successful, where fh is the open file handle
  no       # if not successful
*/
static void reduce2_fopen(value f)
	{
	char *path = string_data(arg(type_string,f->L->R));
	char *mode = string_data(arg(type_string,f->R));

	FILE *fh = fopen(path,mode);
	if (fh)
		replace_apply(f, Q(reduce_yes), Qfile(fh));
	else
		replace(f, Q(reduce_F));
	}

void reduce_fopen(value f)
	{
	f->T = reduce2_fopen;
	}

/* (fdopen fd mode) calls fdopen and returns either (yes fh) or no. */
static void reduce2_fdopen(value f)
	{
	long fd = get_long(arg(type_long,f->L->R));
	char *mode = string_data(arg(type_string,f->R));

	FILE *fh = fdopen(fd,mode);
	if (fh)
		replace_apply(f, Q(reduce_yes), Qfile(fh));
	else
		replace(f, Q(reduce_F));
	}

void reduce_fdopen(value f)
	{
	f->T = reduce2_fdopen;
	}

/* (fgetc file next) = (next ch), where ch is the next character from file. */
static void reduce2_fgetc(value f)
	{
	FILE *fh = get_file(arg(type_file,f->L->R));
	replace_apply(f, f->R, Qlong(fgetc(fh)));
	}

void reduce_fgetc(value f)
	{
	f->T = reduce2_fgetc;
	}

/* (fputc file ch next) */
static void reduce3_fputc(value f)
	{
	FILE *fh = get_file(arg(type_file,f->L->L->R));
	long ch = get_long(arg(type_long,f->L->R));

	fputc(ch, fh);
	replace(f,f->R);
	}

static void reduce2_fputc(value f)
	{
	f->T = reduce3_fputc;
	}

void reduce_fputc(value f)
	{
	f->T = reduce2_fputc;
	}

/* (fwrite file str next) */
static void reduce3_fwrite(value f)
	{
	FILE *fh = get_file(arg(type_file,f->L->L->R));
	value y = arg(type_string,f->L->R);

	size_t count = fwrite(string_data(y), 1, string_len(y), fh);
	(void)count;
	/* LATER make a version of fwrite that returns the count */
	replace(f,f->R);
	}

static void reduce2_fwrite(value f)
	{
	f->T = reduce3_fwrite;
	}

void reduce_fwrite(value f)
	{
	f->T = reduce2_fwrite;
	}

/* (fflush file next) = (next status) */
static void reduce2_fflush(value f)
	{
	FILE *fh = get_file(arg(type_file,f->L->R));
	replace_apply(f, f->R, Qlong(fflush(fh)));
	}

void reduce_fflush(value f)
	{
	f->T = reduce2_fflush;
	}

/* (file_string file next) = (next string) where string is the entire content
of file in a single string. */
static void reduce2_file_string(value f)
	{
	FILE *fh = get_file(arg(type_file,f->L->R));
	struct buf *buf = 0;

	while (1)
		{
		long ch = fgetc(fh);
		if (ch < 0) break;
		buf_add(&buf, ch);
		}

	long len;
	char *data = buf_clear(&buf,&len);
	replace_apply(f, f->R, Qchars(data,len));
	}

void reduce_file_string(value f)
	{
	f->T = reduce2_file_string;
	}

value const_stdin(void)  { return Qfile_const(stdin); }
value const_stdout(void) { return Qfile_const(stdout); }
value const_stderr(void) { return Qfile_const(stderr); }

/* Call readlink, returning a Fexl string. */
value safe_readlink(const char *path)
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
			value result = Qcopy_chars(buf,len);
			free_memory(buf, size);
			return result;
			}
		}
	}

/* (readlink path next) = (next link), where link is the result of calling
readlink(2) on the path. */
static void reduce2_readlink(value f)
	{
	value x = arg(type_string,f->L->R);
	char *path = string_data(x);
	value result = safe_readlink(path);
	replace_apply(f,f->R,result);
	}

void reduce_readlink(value f)
	{
	f->T = reduce2_readlink;
	}

/*
Get the base path of the currently running program, ending in "/".  If we could
not get the path due to a system error, return "".

First we get the full path of the program, for example "/PATH/bin/fexl".  This
is equivalent to the $0 variable in the /bin/sh program.  Then we strip off the
last two legs of that path, returning "/PATH/".
*/
value get_base_path(void)
	{
	value full_path = safe_readlink("/proc/self/exe");
	hold(full_path);

	char *buf = string_data(full_path);
	long len = string_len(full_path);

	int i;
	for (i = 0; i < 2; i++)
		while (len > 0 && buf[--len] != '/')
			;

	if (buf[len] == '/') len++;  /* keep the slash */
	value base_path = Qcopy_chars(buf,len);
	drop(full_path);
	return base_path;
	}

/* (base_path next) = (next path) where path is the base path of the current
Fexl executable. */
void reduce_base_path(value f)
	{
	replace_apply(f, f->R, get_base_path());
	}

/* Concatenate the base path and the name. */
value get_full_path(const char *name)
	{
	value base_path = get_base_path();

	if (string_len(base_path) == 0)
		return base_path;
	else
		{
		value full_path = Qstrcat(string_data(base_path),name);
		hold(base_path);
		drop(base_path);
		return full_path;
		}
	}
