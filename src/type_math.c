#include <value.h>
#include <basic.h>
#include <num.h>
#include <type_math.h>
#include <type_num.h>

static void op_num(value f, number op(number))
	{
	number x = get_num(eval(f->R));
	if (x == 0)
		replace_void(f);
	else
		replace_num(f,op(x));
	}

static void op_num_num(value f, number op(number,number))
	{
	if (f->L->L)
		{
		number x = get_num(eval(f->L->R));
		number y = get_num(eval(f->R));
		if (x == 0 || y == 0)
			replace_void(f);
		else
			replace_num(f,op(x,y));
		}
	}

/*LATER log*/
void type_add(value f) { op_num_num(f,num_add); }
void type_sub(value f) { op_num_num(f,num_sub); }
void type_mul(value f) { op_num_num(f,num_mul); }
void type_div(value f) { op_num_num(f,num_div); }
void type_pow(value f) { op_num_num(f,num_pow); }
void type_round(value f) { op_num(f,num_round); }
void type_trunc(value f) { op_num(f,num_trunc); }
void type_abs(value f) { op_num(f,num_abs); }
void type_sqrt(value f) { op_num(f,num_sqrt); }
