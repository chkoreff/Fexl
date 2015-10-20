#include <str.h>
#include <value.h>
#include <basic.h>
#include <buffer.h>
#include <num.h>
#include <output.h>
#include <stdio.h>
#include <type_buf.h>
#include <type_file.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static void putv(value x)
	{
	x = arg(x);
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
			putv(x->L->R);
			/* Eliminated tail recursive call putv(x->R) here. */
			x = arg(x->R);
			continue;
			}
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
	if (!f->L) return 0;
	putv(f->R); nl();
	return Q(type_I);
	}

value type_flush(value f)
	{
	(void)f;
	flush();
	return Q(type_I);
	}

static buffer *cur_buf = 0;
static void putd_buf(const char *data, unsigned long len)
	{
	buf_addn(cur_buf,data,len);
	}

/* (put_to target content)
Evaluate the content with the current output temporarily set to the target,
which may be a file or a buffer.
LATER Might allow target to be an arbitrary function.
*/
value type_put_to(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	if (x->T == type_file)
		{
		output save_putd = putd;
		void *save_cur_out = cur_out;
		FILE *fh = data(x);

		put_to(fh);
		f = eval(hold(f->R));

		putd = save_putd;
		cur_out = save_cur_out;
		return f;
		}
	else if (x->T == type_buf)
		{
		output save_putd = putd;
		buffer *save_cur_buf = cur_buf;
		buffer *buf = data(x);

		putd = putd_buf;
		cur_buf = buf;

		f = eval(hold(f->R));

		putd = save_putd;
		cur_buf = save_cur_buf;
		return f;
		}
	reduce_void(f);
	return 0;
	}
	}
