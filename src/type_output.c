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

static void putv(value x)
	{
	x = eval(hold(x));
	while (1)
		{
		if (x->T == type_str)
			put_str(data(x));
		else if (x->T == type_num)
			put_num(data(x));
		else if (x->T == type_T)
			put_ch('T');
		else if (x->T == type_F)
			put_ch('F');
		else if (x->T == type_cons && x->L && x->L->L)
			{
			value y;
			putv(x->L->R);
			/* Eliminated tail recursive call putv(x->R) here. */
			y = eval(hold(x->R));
			drop(x);
			x = y;
			continue;
			}
		drop(x);
		return;
		}
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	putv(f->R);
	return Q(type_I);
	}

value type_nl(value f)
	{
	(void)f;
	nl();
	return Q(type_I);
	}

value type_say(value f)
	{
	f = type_put(f);
	if (f) nl();
	return f;
	}

/* (fput fh data) Put data to the file. */
value type_fput(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = eval(hold(f->L->R));
	value y = hold(f->R);
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		int save_cur_out = cur_out;
		cur_out = fileno(fh);
		putv(y);
		cur_out = save_cur_out;
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	drop(y);
	return f;
	}
	}

/* (put_to_error x) = I.  Evaluate x with all output going to stderr. */
value type_put_to_error(value f)
	{
	if (!f->L) return 0;
	{
	int save_cur_out = cur_out;
	put_to_error();
	drop(eval(hold(f->R)));
	f = Q(type_I);
	cur_out = save_cur_out;
	return f;
	}
	}

/* (fflush fh) Force a write of all buffered data to the file handle. */
value type_fflush(value f)
	{
	if (!f->L) return 0;
	{
	value x = eval(hold(f->R));
	if (x->T == type_file)
		{
		FILE *fh = data(x);
		fsync(fileno(fh));
		f = Q(type_I);
		}
	else
		replace_void(f);
	drop(x);
	return f;
	}
	}
