#include <value.h>
#include <basic.h>
#include <num.h>
#include <output.h>
#include <stdio.h>
#include <str.h>
#include <type_num.h>
#include <type_file.h>
#include <type_output.h>
#include <type_str.h>
#include <unistd.h> /* fsync */

static void putv(int out, value x)
	{
	x = arg(x);
	while (1)
		{
		if (x->T == type_str)
			put_str(out,data(x));
		else if (x->T == type_num)
			put_num(out,data(x));
		else if (x->T == type_T)
			put_ch(out,'T');
		else if (x->T == type_F)
			put_ch(out,'F');
		else if (x->T == type_cons && x->L && x->L->L)
			{
			putv(out,x->L->R);
			/* Eliminated tail recursive call putv(out,x->R) here. */
			x = arg(x->R);
			continue;
			}
		return;
		}
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	putv(1,f->R);
	return Q(type_I);
	}

value type_nl(value f)
	{
	(void)f;
	nl(1);
	return Q(type_I);
	}

value type_say(value f)
	{
	if (!f->L) return 0;
	putv(1,f->R); nl(1);
	return Q(type_I);
	}

/* (fput fh data) Put data to the file. */
value type_fput(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		putv(fileno(fh),f->R);
		return Q(type_I);
		}
	reduce_void(f);
	return 0;
	}
	}

/* (error msg) Print the msg followed by newline to stderr. */
value type_error(value f)
	{
	if (!f->L) return 0;
	putv(2,f->R); nl(2);
	return Q(type_I);
	}

/* (fflush fh) Force a write of all buffered data to the file handle. */
value type_fflush(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		fsync(fileno(fh));
		return Q(type_I);
		}
	reduce_void(f);
	return 0;
	}
	}
