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

	value g = D(type_file);
	g->R->T = clear_file;
	g->R->L = (value)fh;

	return A(f->L->R,g);
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
extern void string_write(value x, FILE *stream); /* LATER clean up */
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
