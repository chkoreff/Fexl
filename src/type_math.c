#include <value.h>

#include <num.h>
#include <type_math.h>
#include <type_num.h>

static value op_math(value f, number op(number,number))
	{
	if (!f->L || !f->L->L) return 0;
	value x = arg(f->L->R);
	value y = arg(f->R);
	value z = Qnum(op(get_num(x),get_num(y)));
	drop(x);
	drop(y);
	return z;
	}

value type_add(value f) { return op_math(f,num_add); }
value type_sub(value f) { return op_math(f,num_sub); }
value type_mul(value f) { return op_math(f,num_mul); }
value type_div(value f) { return op_math(f,num_div); }

value type_round0(value f)
	{
	if (!f->L) return 0;
	value x = arg(f->R);
	value y = Qnum(num_round(get_num(x)));
	drop(x);
	return y;
	}
