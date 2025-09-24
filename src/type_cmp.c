#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_cmp.h>
#include <type_num.h>
#include <type_str.h>

static value op_cmp(value f, int op(int))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_num && y->T == type_num)
		f = boolean(op(num_cmp(x->v_double,y->v_double)));
	else if (x->T == type_str && y->T == type_str)
		f = boolean(op(str_cmp(x->v_ptr,y->v_ptr)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

// (compare x y LT EQ GT)
// Compare the numbers or strings and return LT, EQ, or GT.
value type_compare(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	if (f->L->L->L->L == 0) return keep(f);
	if (f->L->L->L->L->L == 0) return keep(f);
	{
	value x = arg(f->L->L->L->L->R);
	value y = arg(f->L->L->L->R);

	if (x->T == type_num && y->T == type_num)
		{
		double xn = x->v_double;
		double yn = y->v_double;

		if (xn < yn)
			f = hold(f->L->L->R);
		else if (xn == yn)
			f = hold(f->L->R);
		else
			f = hold(f->R);
		}
	else if (x->T == type_str && y->T == type_str)
		{
		int cmp = str_cmp(x->v_ptr,y->v_ptr);

		if (cmp < 0)
			f = hold(f->L->L->R);
		else if (cmp == 0)
			f = hold(f->L->R);
		else
			f = hold(f->R);
		}
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
