#include <stdint.h>

#include <stdio.h>
#include <str.h>
#include <types.h>
#include <value.h>

#include <basic.h>
#include <bn.h>
#include <file.h>
#include <file_str.h>
#include <type_bn.h>
#include <type_file.h>
#include <type_num.h>
#include <type_output.h>
#include <type_str.h>

value Qput;
value Qnl;
value Qfput;
value Qfnl;

static value op_put(FILE *fh, value f)
	{
	value x = arg(f->R);
	if (x->T == type_cons && x->L && x->L->L && !x->L->L->L)
		f = A(AV(hold(f->L),hold(x->L->R)),AV(hold(f->L),hold(x->R)));
	else if (x->T == type_list && !x->L->L)
		f = A(AV(hold(f->L),hold(x->R->L)),AV(hold(f->L),wrap(x->R->R)));
	else
		{
		if (x->T == type_str)
			fput_str(fh,get_str(x));
		else if (x->T == type_num)
			fput_double(fh,x->v_double);
		else if (x->T == type_T && !x->L)
			fput_ch(fh,'T');
		else if (x->T == type_F && !x->L)
			fput_ch(fh,'F');
		else if (x->T == type_bn)
			{
			struct bn *n = get_bn(x);
			string s = bn_to_dec(n);
			fput_str(fh,s);
			str_free(s);
			}
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
	return A(AV(hold(Qput),hold(f->R)),hold(Qnl));
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
	return A(A(AV(hold(Qfput),hold(f->L->R)),hold(f->R)),
		AV(hold(Qfnl),hold(f->L->R)));
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

void beg_output(void)
	{
	Qput = Q(type_put);
	Qnl = Q(type_nl);
	Qfput = Q(type_fput);
	Qfnl = Q(type_fnl);
	}

void end_output(void)
	{
	drop(Qput);
	drop(Qnl);
	drop(Qfput);
	drop(Qfnl);
	}
