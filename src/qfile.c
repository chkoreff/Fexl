#include <stdio.h>
#include "file.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "qfile.h"
#include "qstr.h"

value type_file(value f)
	{
	if (!f->N)
		{
		FILE *fh = get_file(f);
		if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
			fclose(fh);
		}
	return f;
	}

FILE *get_file(value f)
	{
	data_type(f,type_file);
	return (FILE *)f->R;
	}

/* Make a file value from the given file handle. */
value Qfile(FILE *fh)
	{
	return V(type_file,0,(value)fh);
	}

/* (putchar ch) Write character to stdout. */
value type_putchar(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	(void)putchar(get_long(x));
	drop(x);
	return I;
	}

/* getchar : Return next char from stdin. */
value type_getchar(value f)
	{
	return Qlong(getchar());
	}

value type_stdin(value f)  { return Qfile(stdin); }
value type_stdout(value f) { return Qfile(stdout); }
value type_stderr(value f) { return Qfile(stderr); }

/* (fputc file ch) Write character to stdout. */
value type_fputc(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);

	FILE *fh = get_file(x);
	(void)fputc(get_long(y),fh);

	drop(x);
	drop(y);
	return I;
	}

/* (fgetc fh) returns the next character from the file handle. */
value type_fgetc(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);

	FILE *fh = get_file(x);
	int ch = fgetc(fh);

	drop(x);
	return Qlong(ch);
	}

/* (fwrite fh str) Write string to file handle. */
value type_fwrite(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);

	FILE *fh = get_file(x);
	struct str *str = get_str(y);
	size_t count = fwrite(str->data, 1, str->len, fh);
	(void)count;  /* Ignore the return count. */

	drop(x);
	drop(y);
	return I;
	}

/* (fopen path mode) Open a file and return no or (yes fh). */
value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);

	struct str *path = get_str(x);
	struct str *mode = get_str(y);

	FILE *fh = fopen(path->data, mode->data);
	value g;
	if (fh)
		g = A(C,yield(I,Qfile(fh)));
	else
		g = C;

	drop(x);
	drop(y);
	return g;
	}

value type_readlink(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qstr(safe_readlink(get_str(x)->data));
	drop(x);
	return y;
	}

/*LATER more functions */

/*
LATER fclose : (not necessary due to auto-close, but gives you some control.
Set the right side FILE * to 0 afterward.
*/

/*LATER fdopen */
/*LATER fflush */
/*LATER file_string */
/*LATER fmemopen */

#if 0

/* sample code here */
{
char *s = "abcdefg";
FILE *fp = fmemopen(s, strlen(s), "r");
int ch;
while (1)
	{
	ch = fgetc(fp);
	if (ch == -1) break;
	printf("ch='%c'\n", ch);
	}
fclose(fp);
}
#endif
