#include <stdint.h>

#include <str.h>
#include <types.h>
#include <value.h>

#include <basic.h>
#include <bn.h>
#include <type_bn.h>
#include <type_num.h>
#include <type_str.h>
#include <type_tuple.h>

struct bn *get_bn(value x)
	{
	return x->v_ptr;
	}

value type_bn(value fun, value f)
	{
	return type_void(fun,f);
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

static value op_pred(value fun, value f, int op(const struct bn *x))
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = boolean(op(get_bn(x)));
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value type_bn_eq0(value fun, value f)
	{
	return op_pred(fun,f,bn_eq0);
	}

value type_bn_is_neg(value fun, value f)
	{
	return op_pred(fun,f,bn_is_neg);
	}

value type_bn_neg(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qbn(bn_neg(get_bn(x)));
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value type_bn_cmp(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qnum(bn_cmp(get_bn(x),get_bn(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

static value op_cmp(value fun, value f, int op(int))
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = boolean(op(bn_cmp(get_bn(x),get_bn(y))));
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

value type_bn_lt(value fun, value f) { return op_cmp(fun,f,lt); }
value type_bn_le(value fun, value f) { return op_cmp(fun,f,le); }
value type_bn_eq(value fun, value f) { return op_cmp(fun,f,eq); }
value type_bn_ne(value fun, value f) { return op_cmp(fun,f,ne); }
value type_bn_ge(value fun, value f) { return op_cmp(fun,f,ge); }
value type_bn_gt(value fun, value f) { return op_cmp(fun,f,gt); }

value type_bn_from_dec(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_str)
		f = Qbn(bn_from_dec(str_data(x)));
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

value type_bn_to_dec(value fun, value f)
	{
	value x = arg(f->R);
	if (x->T == type_bn)
		f = Qstr(bn_to_dec(get_bn(x)));
	else
		f = hold(Qvoid);
	drop(x);
	(void)fun;
	return f;
	}

static value op_2(value fun, value f,
	struct bn *op(const struct bn *, const struct bn *))
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		f = Qbn(op(get_bn(x),get_bn(y)));
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_bn_add(value fun, value f) { return op_2(fun,f,bn_add); }
value type_bn_sub(value fun, value f) { return op_2(fun,f,bn_sub); }
value type_bn_mul(value fun, value f) { return op_2(fun,f,bn_mul); }
value type_bn_mod(value fun, value f) { return op_2(fun,f,bn_mod); }

value type_bn_div(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	{
	value x = arg(fun->R);
	value y = arg(f->R);
	if (x->T == type_bn && y->T == type_bn)
		{
		struct bn *q;
		struct bn *r;
		bn_div(get_bn(x),get_bn(y),&q,&r);
		f = pair(Qbn(q),Qbn(r));
		}
	else
		f = hold(Qvoid);
	drop(x);
	drop(y);
	return f;
	}
	}

value type_is_bn(value fun, value f)
	{
	return op_is_type(fun,f,type_bn);
	}
