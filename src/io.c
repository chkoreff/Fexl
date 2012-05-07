#include <stdio.h>
#include <unistd.h>
#include "value.h"
#include "long.h"
#include "string.h"

value fexl_nl(value f)
	{
	if (!f->L) return 0;
	putchar('\n');
	return f->R;
	}

/* Note that we use fwrite here because we may be writing binary data.  */
void string_write(value x, FILE *stream)
	{
	fwrite(string_data(x), 1, string_len(x), stream);
	}

static value op_string_write(value f, FILE *stream)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_string,x)) return 0;

	string_write(x, stream);
	return f->R;
	}

/* string_put str next */
value fexl_string_put(value f)
	{
	return op_string_write(f, stdout);
	}

/* string_stderr str next */
value fexl_string_stderr(value f)
	{
	return op_string_write(f, stderr);
	}

/* (char_get next) = (next ch), where ch is the next character from stdin. */
value fexl_char_get(value f)
	{
	if (!f->L) return 0;
	return A(f->R,Qlong(getchar()));
	}

/* char_put ch next */
value fexl_char_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	putchar(get_long(x));
	return f->R;
	}

/*
(pipe next) = (next status read write), where read and write are the two ends
of the pipe, expressed as numeric file descriptors.  The status is the return
value of pipe, 0 if success, or -1 otherwise.
*/
value fexl_pipe(value f)
	{
	if (!f->L) return 0;
	int fd[2];
	int status = pipe(fd);
	return A(A(A(f->R,Qlong(status)),Qlong(fd[0])),Qlong(fd[1]));
	}

/* (dup2 oldfd newfd next) = (next status) */
value fexl_dup2(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	value x = f->L->L->R;
	value y = f->L->R;
	if (!arg(type_long,x)) return 0;
	if (!arg(type_long,y)) return 0;

	long oldfd = get_long(x);
	long newfd = get_long(y);

	int status = dup2(oldfd, newfd);
	return A(f->R,Qlong(status));
	}

/* (close fd next) = (next status) */
value fexl_close(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value x = f->L->R;
	if (!arg(type_long,x)) return 0;

	long fd = get_long(x);
	long status = close(fd);
	return A(f->R,Qlong(status));
	}
