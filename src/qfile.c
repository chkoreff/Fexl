#include <stdio.h>
#include "file.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "long.h"
#include "qfile.h"
#include "qstr.h"

FILE *as_file(value f)
	{
	return (FILE *)f->R;
	}

value type_file(value f)
	{
	if (!f->N)
		{
		FILE *fh = as_file(f);
		if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
			fclose(fh);
		}
	return f;
	}

FILE *get_file(value f)
	{
	if (f->T != type_file) bad_type();
	return as_file(f);
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

value type_base_path(value f)
	{
	return Qstr(base_path());
	}
