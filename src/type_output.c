#include <num.h>
#include <stdio.h>
#include <str.h>
#include <value.h>
#include <basic.h>
#include <file.h>
#include <file2.h>
#include <type_file.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static void fputv(FILE *fh, value x)
	{
	x = arg(x);
	while (1)
		{
		if (x->T == type_str)
			fput_str(fh,data(x));
		else if (x->T == type_num)
			fput_num(fh,data(x));
		else if (x->T == type_T && !x->L)
			fput_ch(fh,'T');
		else if (x->T == type_F && !x->L)
			fput_ch(fh,'F');
		else if (x->T == type_cons && x->L && x->L->L)
			{
			fputv(fh,x->L->R);
			/* Eliminated tail recursive call fputv(fh,x->R) here. */
			{
			value y = arg(x->R);
			drop(x);
			x = y;
			continue;
			}
			}
		drop(x);
		return;
		}
	}

static void fsayv(FILE *fh, value x)
	{
	fputv(fh,x);
	fnl(fh);
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	fputv(stdout,f->R);
	return QI();
	}

value type_nl(value f)
	{
	(void)f;
	fnl(stdout);
	return QI();
	}

value type_say(value f)
	{
	if (!f->L) return 0;
	fsayv(stdout,f->R);
	return QI();
	}

static value op_output(value f, void put(FILE *fh, value x))
	{
	if (!f->L || !f->L->L) return 0;
	{
	value out = arg(f->L->R);
	if (out->T == type_file)
		{
		FILE *fh = data(out);
		put(fh,f->R);
		f = QI();
		}
	else
		reduce_void(f);
	drop(out);
	return f;
	}
	}

value type_fput(value f)
	{
	return op_output(f,fputv);
	}

value type_fsay(value f)
	{
	return op_output(f,fsayv);
	}

value type_fflush(value f)
	{
	if (!f->L) return 0;
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		FILE *fh = data(out);
		fflush(fh);
		f = QI();
		}
	else
		reduce_void(f);
	drop(out);
	return f;
	}
	}
