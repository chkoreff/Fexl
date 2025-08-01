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
	if (x->T == type_list)
		f = A(A(hold(f->L),hold(x->L)),A(hold(f->L),hold(x->R)));
	else
		{
		if (x->T == type_str)
			fput_str(fh,x->v_ptr);
		else if (x->T == type_num)
			fput_double(fh,x->v_double);
		else if (x->T == type_T && !x->L)
			fput_ch(fh,'T');
		else if (x->T == type_F && !x->L)
			fput_ch(fh,'F');
		else if (x->T == type_bn)
			{
			string s = bn_to_dec(x->v_ptr);
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
	return op_put(stdout,f);
	}

value type_nl(value f)
	{
	nl();
	return hold(QI);
	(void)f;
	}

value type_say(value f)
	{
	return A(A(hold(Qput),hold(f->R)),hold(Qnl));
	}

value type_fput(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value out = arg(f->L->R);
	if (out->T == type_file)
		f = op_put(out->v_ptr,f);
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
	}

value type_fnl(value f)
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		fnl(out->v_ptr);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}

value type_fsay(value f)
	{
	if (f->L->L == 0) return keep(f);
	return A(A(A(hold(Qfput),hold(f->L->R)),hold(f->R)),
		A(hold(Qfnl),hold(f->L->R)));
	}

value type_fflush(value f)
	{
	value out = arg(f->R);
	if (out->T == type_file)
		{
		fflush(out->v_ptr);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(out);
	return f;
	}
