#include <stdio.h>
#include "file.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "double.h"
#include "long.h"
#include "qfile.h"
#include "qstr.h"
#include "var.h"

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
value fexl_putchar(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	(void)putchar(get_long(x));
	drop(x);
	return I;
	}

/* getchar : Return next char from stdin. */
value fexl_getchar(value f)
	{
	return Qlong(getchar());
	}

value const_fexl_stdin(void)  { return Qfile(stdin); }
value const_fexl_stdout(void) { return Qfile(stdout); }
value const_fexl_stderr(void) { return Qfile(stderr); }

/* (fputc file ch) Write character to stdout. */
value fexl_fputc(value f)
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
value fexl_fgetc(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);

	FILE *fh = get_file(x);
	int ch = fgetc(fh);

	drop(x);
	return Qlong(ch);
	}

/* (fwrite fh str) Write string to file handle. */
value fexl_fwrite(value f)
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
value fexl_fopen(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	value y = eval(f->R);

	struct str *path = get_str(x);
	struct str *mode = get_str(y);

	FILE *fh = fopen(path->data, mode->data);
	value result = maybe(fh ? Qfile(fh) : 0);

	drop(x);
	drop(y);
	return result;
	}

value fexl_readlink(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qstr(safe_readlink(get_str(x)->data));
	drop(x);
	return y;
	}

value const_fexl_base_path(void) { return Qstr(base_path()); }

static FILE *curr_fh;

/* I removed the tail recursion in the cases for fexl_item and type_var. */
static void fprint(value f)
	{
	while (1)
		{
		if (f->T == type_string)
			{
			struct str *str = get_str(f);
			size_t count = fwrite(str->data, 1, str->len, curr_fh);
			(void)count;  /* Ignore the return count. */
			}
		else if (f->T == type_long)
			fprintf(curr_fh, "%ld", get_long(f));
		else if (f->T == type_double)
			fprintf(curr_fh, "%.15g", get_double(f));
		else if (f->T == fexl_item)
			{
			if (!f->L || !f->L->L) bad_type();
			fprint(eval(f->L->R));
			drop(f);
			f = eval(f->R);
			continue;
			}
		else if (f->T == fexl_C)
			;
		else if (f->T == type_var)
			{
			drop(f);
			f = eval(f->R);
			continue;
			}
		else
			bad_type();

		drop(f);
		break;
		}
	}

value fexl_fprint(value f)
	{
	if (!f->L || !f->L->L) return f;
	value x = eval(f->L->R);
	FILE *save_fh = curr_fh;
	curr_fh = get_file(x);
	fprint(eval(f->R));
	curr_fh = save_fh;
	drop(x);
	return I;
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
