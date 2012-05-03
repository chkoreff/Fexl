#include <stdio.h>
#include "buf.h"
#include "value.h"
#include "eval.h"
#include "io.h"
#include "long.h"
#include "string.h"

value type_file(value f) { return f; }

/* Close the file when it's no longer used. */
static value clear_file(value f)
	{
	FILE *fh = (FILE *)f->R->L;
	if (fh) fclose(fh);
	return f;
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
	f->R->T = 0;
	return f;
	}

/* (file_open path mode yes no) opens the file with the given path and returns:

  yes fh       # if successful, where fh is the open file handle
  no           # if not successful
*/
value fexl_fopen(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return f;

	value x = f->L->L->L->R;
	value y = f->L->L->R;
	if (!arg(type_string,x)) return f;
	if (!arg(type_string,y)) return f;

	char *path = string_data(x);
	char *mode = string_data(y);

	FILE *fh = fopen(path,mode);
	if (fh == 0) return f->R;
	return A(f->L->R,Qfile(fh));
	}

/* (fdopen fd mode yes no) calls fdopen on fd and mode and returns either
(yes fh) or no.
*/
value fexl_fdopen(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	value x = f->L->L->L->R;
	value y = f->L->L->R;
	if (!arg(type_long,x)) return f;
	if (!arg(type_string,y)) return f;

	long fd = get_long(x);
	char *mode = string_data(y);

	FILE *fh = fdopen(fd,mode);
	if (fh == 0) return f->R;
	return A(f->L->R,Qfile(fh));
	}

/* (fgetc file next) = (next ch), where ch is the next character from file. */
value fexl_fgetc(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_file,x)) return f;

	FILE *fh = (FILE *)x->R->L;

	return A(f->R,Qlong(fgetc(fh)));
	}

/* (fputc file ch next) */
value fexl_fputc(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	value y = f->L->R;
	if (!arg(type_file,x)) return f;
	if (!arg(type_long,y)) return f;

	FILE *fh = (FILE *)x->R->L;
	fputc(get_long(y), fh);
	return f->R;
	}

/* (fwrite file str next) */
extern void string_write(value x, FILE *stream);
value fexl_fwrite(value f)
	{
	if (!f->L->L || !f->L->L->L) return f;

	value x = f->L->L->R;
	value y = f->L->R;
	if (!arg(type_file,x)) return f;
	if (!arg(type_string,y)) return f;

	FILE *fh = (FILE *)x->R->L;
	string_write(y, fh);
	return f->R;
	}

/* (fflush file next) = (next status) */
value fexl_fflush(value f)
	{
	if (!f->L->L) return f;
	value x = f->L->R;
	if (!arg(type_file,x)) return f;
	FILE *fh = (FILE *)x->R->L;
	return A(f->R,Qlong(fflush(fh)));
	}

/* (file_string file next) = (next string) where string is the entire content
of file in a single string. */
value fexl_file_string(value f)
	{
	if (!f->L->L) return f;

	value x = f->L->R;
	if (!arg(type_file,x)) return f;

	FILE *fh = (FILE *)x->R->L;
	struct buf *buf = 0;

	while (1)
		{
		long ch = fgetc(fh);
		if (ch < 0) break;
		buf_add(&buf, ch);
		}

	long len;
	char *data = buf_clear(&buf,&len);
	return A(f->R,Qchars(data,len));
	}

value fexl_get_stdin(value f)
	{
	return A(f->R,Qfile_const(stdin));
	}

value fexl_get_stdout(value f)
	{
	return A(f->R,Qfile_const(stdout));
	}

value fexl_get_stderr(value f)
	{
	return A(f->R,Qfile_const(stderr));
	}
