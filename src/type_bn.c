#include <stdint.h>

#include <str.h>
#include <types.h>
#include <value.h>

#include <basic.h>
#include <bn.h>
#include <type_bn.h>
#include <type_num.h>
#include <type_str.h>

value type_bn(value f)
	{
	return type_void(f);
	}

static void clear_bn(value f)
	{
	bn_free(f->v_ptr);
	}

value Qbn(struct bn *x)
	{
	static struct value clear = {{.N=0}, {.clear=clear_bn}};
	return V(type_bn,&clear,(value)x);
	}

static value op_pred(value f, int op(const struct bn *x))
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = boolean(op(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

value type_bn_eq0(value f)
	{
	return op_pred(f,bn_eq0);
	}

value type_bn_is_neg(value f)
	{
	return op_pred(f,bn_is_neg);
	}

value type_bn_neg(value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qbn(bn_neg(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

value type_bn_cmp(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qnum(bn_cmp(x->v_ptr,y->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value op_cmp(value f, int op(int))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = boolean(op(bn_cmp(x->v_ptr,y->v_ptr)));
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

value type_bn_lt(value f) { return op_cmp(f,lt); }
value type_bn_le(value f) { return op_cmp(f,le); }
value type_bn_eq(value f) { return op_cmp(f,eq); }
value type_bn_ne(value f) { return op_cmp(f,ne); }
value type_bn_ge(value f) { return op_cmp(f,ge); }
value type_bn_gt(value f) { return op_cmp(f,gt); }

value type_bn_from_dec(value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qbn(bn_from_dec(str_data(x)));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

value type_bn_to_dec(value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qstr(bn_to_dec(x->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

static value op_2(value f,
	struct bn *op(const struct bn *, const struct bn *))
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qbn(op(x->v_ptr,y->v_ptr));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_bn_add(value f) { return op_2(f,bn_add); }
value type_bn_sub(value f) { return op_2(f,bn_sub); }
value type_bn_mul(value f) { return op_2(f,bn_mul); }
value type_bn_mod(value f) { return op_2(f,bn_mod); }

value type_bn_div(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		{
		struct bn *q;
		struct bn *r;
		bn_div(x->v_ptr,y->v_ptr,&q,&r);
		f = pair(Qbn(q),Qbn(r));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_is_bn(value f)
	{
	return op_is_type(f,type_bn);
	}
