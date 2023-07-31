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

static value op_cmp(value fun, value f, int op(int))
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_num && y->T == type_num)
		f = boolean(op(num_cmp(x->v_double,y->v_double)));
	else if (x->T == type_str && y->T == type_str)
		f = boolean(op(str_cmp(get_str(x),get_str(y))));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

// (compare x y LT EQ GT)
// Compare the numbers or strings and return LT, EQ, or GT.
value type_compare(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	if (fun->L->L->L == 0) return keep(fun,f);
	if (fun->L->L->L->L == 0) return keep(fun,f);
	{
	value x = arg(fun->L->L->L->R);
	value y = arg(fun->L->L->R);

	if (x->T == type_num && y->T == type_num)
		{
		double xn = x->v_double;
		double yn = y->v_double;

		if (xn < yn)
			f = hold(fun->L->R);
		else if (xn == yn)
			f = hold(fun->R);
		else
			f = hold(f->R);
		}
	else if (x->T == type_str && y->T == type_str)
		{
		int cmp = str_cmp(get_str(x),get_str(y));

		if (cmp < 0)
			f = hold(fun->L->R);
		else if (cmp == 0)
			f = hold(fun->R);
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

value type_lt(value fun, value f) { return op_cmp(fun,f,lt); }
value type_le(value fun, value f) { return op_cmp(fun,f,le); }
value type_eq(value fun, value f) { return op_cmp(fun,f,eq); }
value type_ne(value fun, value f) { return op_cmp(fun,f,ne); }
value type_ge(value fun, value f) { return op_cmp(fun,f,ge); }
value type_gt(value fun, value f) { return op_cmp(fun,f,gt); }
