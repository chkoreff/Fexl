#include <value.h>
#include <basic.h>
#include <num.h>
#include <str.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static void op_cmp(value f, int op(int))
	{
	if (f->L->L)
		{
		value x = eval(f->L->R);
		value y = eval(f->R);
		if (x->T == type_num && y->T == type_num)
			replace_boolean(f, op(num_cmp(get_num(x),get_num(y))));
		else if (x->T == type_str && y->T == type_str)
			replace_boolean(f, op(str_cmp(get_str(x),get_str(y))));
		else
			replace_void(f);
		}
	}

static int lt(int x) { return x < 0; }
static int le(int x) { return x <= 0; }
static int eq(int x) { return x == 0; }
static int ne(int x) { return x != 0; }
static int ge(int x) { return x >= 0; }
static int gt(int x) { return x > 0; }

void type_lt(value f) { op_cmp(f,lt); }
void type_le(value f) { op_cmp(f,le); }
void type_eq(value f) { op_cmp(f,eq); }
void type_ne(value f) { op_cmp(f,ne); }
void type_ge(value f) { op_cmp(f,ge); }
void type_gt(value f) { op_cmp(f,gt); }
