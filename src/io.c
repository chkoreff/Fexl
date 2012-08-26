#include <stdio.h>
#include <unistd.h>
#include "value.h"
#include "long.h"
#include "string.h"

void reduce_nl(value f)
	{
	putchar('\n');
	replace(f,f->R);
	}

/* (char_get next) = (next ch), where ch is the next character from stdin. */
void reduce_char_get(value f)
	{
	replace_apply(f, f->R, Qlong(getchar()));
	}

/* char_put ch next */
static void reduce2_char_put(value f)
	{
	putchar(get_long(arg(type_long,f->L->R)));
	replace(f,f->R);
	}

void reduce_char_put(value f)
	{
	f->T = reduce2_char_put;
	}

/*
(pipe next) = (next status read write), where read and write are the two ends
of the pipe, expressed as numeric file descriptors.  The status is the return
value of pipe, 0 if success, or -1 otherwise.
*/
void reduce_pipe(value f)
	{
	int fd[2];
	int status = pipe(fd);
	replace_apply(f, A(A(f->R,Qlong(status)),Qlong(fd[0])), Qlong(fd[1]));
	}

/* (dup2 oldfd newfd next) = (next status) */
static void reduce3_dup2(value f)
	{
	long oldfd = get_long(arg(type_long,f->L->L->R));
	long newfd = get_long(arg(type_long,f->L->R));

	int status = dup2(oldfd, newfd);
	replace_apply(f, f->R, Qlong(status));
	}

static void reduce2_dup2(value f)
	{
	f->T = reduce3_dup2;
	}

void reduce_dup2(value f)
	{
	f->T = reduce2_dup2;
	}

/* (close fd next) = (next status) */
static void reduce2_close(value f)
	{
	long fd = get_long(arg(type_long,f->L->R));
	long status = close(fd);
	replace_apply(f, f->R, Qlong(status));
	}

void reduce_close(value f)
	{
	f->T = reduce2_close;
	}
