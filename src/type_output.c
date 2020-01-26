#include <stdio.h>
#include <str.h>
#include <value.h>

#include <basic.h>
#include <file.h>
#include <file_str.h>
#include <standard.h>
#include <type_file.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

static value op_put(FILE *fh, value f)
	{
	value x = arg(f->R);
	if (x->T == type_cons && x->L && x->L->L)
		f = A(A(hold(f->L),hold(x->L->R)),A(hold(f->L),hold(x->R)));
	else
		{
		if (x->T == type_str)
			fput_str(fh,get_str(x));
		else if (x->T == type_num)
			fput_double(fh,get_double(x));
		else if (x->T == type_T && !x->L)
			fput_ch(fh,'T');
		else if (x->T == type_F && !x->L)
			fput_ch(fh,'F');

		f = hold(QI);
		}
	drop(x);
	return f;
	}

value type_put(value f)
	{
	if (!f->L) return 0;
	return op_put(stdout,f);
	}

value type_nl(value f)
	{
	(void)f;
	fnl(stdout);
	return hold(QI);
	}

value type_say(value f)
	{
	if (!f->L) return 0;
	return A(A(hold(Qput),hold(f->R)),hold(Qnl));
	}

value type_fput(value f)
	{
	if (!f->L || !f->L->L) return 0;
	{
	value out = arg(f->L->R);
	if (out->T == type_file)
		f = op_put(get_fh(out),f);
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}

value type_fnl(value f)
	{
	if (!f->L) return 0;
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		fnl(get_fh(out));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}

value type_fsay(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return A(A(A(hold(Qfput),hold(f->L->R)),hold(f->R)),
		A(hold(Qfnl),hold(f->L->R)));
	}

value type_fflush(value f)
	{
	if (!f->L) return 0;
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		fflush(get_fh(out));
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}
