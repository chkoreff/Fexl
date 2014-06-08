#include <value.h>

#include <num.h>
#include <type_math.h>
#include <type_num.h>

static value op_num(value f, number op(number))
	{
	value x, y;
	if (!f->L) return 0;
	x = arg(f->R);
	y = Qnum(op(get_num(x)));
	drop(x);
	return y;
	}

static value op_num_num(value f, number op(number,number))
	{
	value x, y, z;
	if (!f->L || !f->L->L) return 0;
	x = arg(f->L->R);
	y = arg(f->R);
	z = Qnum(op(get_num(x),get_num(y)));
	drop(x);
	drop(y);
	return z;
	}

value type_add(value f) { return op_num_num(f,num_add); }
value type_sub(value f) { return op_num_num(f,num_sub); }
value type_mul(value f) { return op_num_num(f,num_mul); }
value type_div(value f) { return op_num_num(f,num_div); }
value type_pow(value f) { return op_num_num(f,num_pow); }
value type_round(value f) { return op_num(f,num_round); }
value type_trunc(value f) { return op_num(f,num_trunc); }
value type_abs(value f) { return op_num(f,num_abs); }
