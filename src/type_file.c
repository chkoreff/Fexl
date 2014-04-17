#include <value.h>
#include <basic.h>
#include <file.h>
#include <stdio.h>
#include <str.h>
#include <type_double.h>
#include <type_long.h>
#include <type_file.h>
#include <type_str.h>
#include <type_var.h>

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
	return (FILE *)get_data(f,type_file);
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
	(void)f;
	return Qlong(getchar());
	}

/* (fputc file ch) Write character to the file. */
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
	value result = maybe(fh ? Qfile(fh) : 0);

	drop(x);
	drop(y);
	return result;
	}

value type_readlink(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qstr(safe_readlink(get_str(x)->data));
	drop(x);
	return y;
	}

/* I removed the tail recursion in the cases for type_item and type_var. */
void print(value f)
	{
	while (1)
		{
		if (f->T == type_str)
			{
			struct str *str = get_str(f);
			size_t count = fwrite(str->data, 1, str->len, stdout);
			(void)count;  /* Ignore the return count. */
			}
		else if (f->T == type_long)
			printf("%ld", get_long(f));
		else if (f->T == type_double)
			printf("%.15g", get_double(f));
		else if (f->T == type_cons)
			{
			if (!f->L || !f->L->L) bad_type();
			print(eval(f->L->R));
			drop(f);
			f = eval(f->R);
			continue;
			}
		else if (f->T == type_C)
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

void nl(void)
	{
	putchar('\n');
	}

value type_nl(value f)
	{
	(void)f;
	nl();
	return I;
	}

value type_print(value f)
	{
	if (!f->L) return f;
	print(eval(f->R));
	return I;
	}

value type_say(value f)
	{
	if (!f->L) return f;
	print(eval(f->R)); nl();
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
