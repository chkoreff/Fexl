#include <stdio.h>
#include "file.h"
#include "str.h"

#include "value.h"
#include "basic.h"
#include "qfile.h"
#include "qstr.h"

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
	value g = fh ? Qfile(fh) : 0; /*TODO not quite */

	drop(x);
	drop(y);
	return g; /*TODO*/
	}

value type_readlink(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qstr(safe_readlink(get_str(x)->data));
	drop(x);
	return y;
	}

/*TODO more functions */

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
