#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static int num_cmp(double x, double y)
	{
	if (x < y) return -1;
	if (x > y) return 1;
	return 0;
	}

static value op_cmp(value f, int op(int))
	{
	if (!f->L || !f->L->L) return 0;
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_num && y->T == type_num)
		f = boolean(op(num_cmp(get_double(x),get_double(y))));
	else if (x->T == type_str && y->T == type_str)
		f = boolean(op(str_cmp(get_str(x),get_str(y))));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static int lt(int x) { return x < 0; }
static int le(int x) { return x <= 0; }
static int eq(int x) { return x == 0; }
static int ne(int x) { return x != 0; }
static int ge(int x) { return x >= 0; }
static int gt(int x) { return x > 0; }

value type_lt(value f) { return op_cmp(f,lt); }
value type_le(value f) { return op_cmp(f,le); }
value type_eq(value f) { return op_cmp(f,eq); }
value type_ne(value f) { return op_cmp(f,ne); }
value type_ge(value f) { return op_cmp(f,ge); }
value type_gt(value f) { return op_cmp(f,gt); }
