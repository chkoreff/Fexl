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

static value op_put(FILE *fh, value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_list)
		f = A(A(hold(fun),hold(x->R->L)),A(hold(fun),wrap(x->R->R)));
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

value type_put(value fun, value f)
	{
	return op_put(stdout,fun,f);
	}

value type_nl(value fun, value f)
	{
	(void)fun;
	(void)f;
	fnl(stdout);
	return hold(QI);
	}

value type_say(value fun, value f)
	{
	(void)fun;
	return A(A(hold(Qput),hold(f->R)),hold(Qnl));
	}

value type_fput(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value out = arg(fun->R);
	if (out->T == type_file)
		f = op_put(get_fh(out),fun,f);
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}

value type_fnl(value fun, value f)
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
	(void)fun;
	return f;
	}

value type_fsay(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	return A(A(A(hold(Qfput),hold(fun->R)),hold(f->R)),
		A(hold(Qfnl),hold(fun->R)));
	}

value type_fflush(value fun, value f)
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
	(void)fun;
	return f;
	}

void beg_output(void)
	{
	Qput = Q(type_put);
	Qnl = Q0(type_nl);
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
