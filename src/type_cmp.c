#include <num.h>

#include <value.h>
#include <basic.h>
#include <str.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static int cmp(value x, value y)
	{
	if (x->L || y->L) bad_type();

	if (x->T == type_num && y->T == type_num)
		return num_cmp((number)x->R,(number)y->R);
	else if (x->T == type_str && y->T == type_str)
		return str_cmp((string)x->R,(string)y->R);

	bad_type();
	return 0;
	}

static value op_cmp(value f, int op(value,value))
	{
	if (!f->L || !f->L->L) return 0;
	value x = arg(f->L->R);
	value y = arg(f->R);
	value z = Qboolean(op(x,y));
	drop(x);
	drop(y);
	return z;
	}

static int lt(value x, value y) { return cmp(x,y) < 0; }
static int le(value x, value y) { return cmp(x,y) <= 0; }
static int eq(value x, value y) { return cmp(x,y) == 0; }
static int ne(value x, value y) { return cmp(x,y) != 0; }
static int ge(value x, value y) { return cmp(x,y) >= 0; }
static int gt(value x, value y) { return cmp(x,y) > 0; }

value type_lt(value f) { return op_cmp(f,lt); }
value type_le(value f) { return op_cmp(f,le); }
value type_eq(value f) { return op_cmp(f,eq); }
value type_ne(value f) { return op_cmp(f,ne); }
value type_ge(value f) { return op_cmp(f,ge); }
value type_gt(value f) { return op_cmp(f,gt); }
