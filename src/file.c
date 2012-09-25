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

	fwrite(string_data(y), 1, string_len(y), fh);
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

/*
Safely call readlink, returning a NUL-terminated result in a dynamically
allocated buffer.

Note that the size of the buffer is always a power of two.  It doesn't have to
be that way, but it feels right.
*/
void safe_readlink(const char *path,
	char **result, long *result_len, long *result_size)
	{
	long size = 256;
	while (1)
		{
		char *link = new_memory(size);
		ssize_t len = readlink(path, link, size - 1);
		if (len == -1)
			{
			free_memory(link, size);
			*result = 0;
			*result_len = 0;
			*result_size = 0;
			return;
			}
		else if (len == size - 1)
			{
			/* Assume the worst:  the result might be truncated. */
			free_memory(link, size);
			size = 2 * size;
			}
		else
			{
			link[len] = 0;
			*result = link;
			*result_len = len;
			*result_size = size;
			return;
			}
		}
	}

/* readlink path next = (next link), where link is the result of calling
readlink(2) on the path. */
static void reduce2_readlink(value f)
	{
	value x = arg(type_string,f->L->R);
	char *path = string_data(x);

	char *buf;
	long len;
	long size;
	safe_readlink(path, &buf, &len, &size);

	value result = Qcopy_chars(buf,len);
	if (buf) free_memory(buf, size);

	replace_apply(f,f->R,result);
	}

void reduce_readlink(value f)
	{
	f->T = reduce2_readlink;
	}
