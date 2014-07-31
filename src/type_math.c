#include <value.h>
#include <basic.h>
#include <num.h>
#include <type_math.h>
#include <type_num.h>

static value op_num(value f, number op(number))
	{
	if (!f->L) return 0;
	{
	number x = atom(type_num,arg(&f->R));
	if (!x) return bad;
	return Qnum(op(x));
	}
	}

static value op_num_num(value f, number op(number,number))
	{
	if (!f->L || !f->L->L) return 0;
	{
	number x = atom(type_num,arg(&f->L->R));
	number y = atom(type_num,arg(&f->R));
	if (!x || !y) return bad;
	return Qnum(op(x,y));
	}
	}

/*LATER log*/
value type_add(value f) { return op_num_num(f,num_add); }
value type_sub(value f) { return op_num_num(f,num_sub); }
value type_mul(value f) { return op_num_num(f,num_mul); }
value type_div(value f) { return op_num_num(f,num_div); }
value type_pow(value f) { return op_num_num(f,num_pow); }
value type_round(value f) { return op_num(f,num_round); }
value type_trunc(value f) { return op_num(f,num_trunc); }
value type_abs(value f) { return op_num(f,num_abs); }
value type_sqrt(value f) { return op_num(f,num_sqrt); }
