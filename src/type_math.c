#include <value.h>
#include <basic.h>
#include <num.h>
#include <type_math.h>
#include <type_num.h>

static value op_num(value f, number op(number))
	{
	if (!f->L) return f;
	{
	value x = eval(hold(f->R));
	value g = 0;
	if (is_atom(type_num,x))
		g = Qnum(op((number)x->R));
	drop(x);
	return g;
	}
	}

static value op_num_num(value f, number op(number,number))
	{
	if (!f->L || !f->L->L) return f;
	{
	value x = eval(hold(f->L->R));
	value y = eval(hold(f->R));
	value g = 0;
	if (is_atom(type_num,x) && is_atom(type_num,y))
		g = Qnum(op((number)x->R,(number)y->R));
	drop(x);
	drop(y);
	return g;
	}
	}

value type_add(value f) { return op_num_num(f,num_add); }
value type_sub(value f) { return op_num_num(f,num_sub); }
value type_mul(value f) { return op_num_num(f,num_mul); }
value type_div(value f) { return op_num_num(f,num_div); }
value type_pow(value f) { return op_num_num(f,num_pow); }
value type_round(value f) { return op_num(f,num_round); }
value type_trunc(value f) { return op_num(f,num_trunc); }
value type_abs(value f) { return op_num(f,num_abs); }
value type_sqrt(value f) { return op_num(f,num_sqrt); }
value type_log(value f) { return op_num(f,num_log); }
value type_sin(value f) { return op_num(f,num_sin); }
value type_cos(value f) { return op_num(f,num_cos); }
