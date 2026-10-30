#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_num.h>
#include <type_vector.h>

struct vector
	{
	unsigned long len; // number of values in data
	unsigned long size; // maximum number of values in data
	value *data;
	};

// Reset vector to empty.
static void reset_vector(struct vector *vec)
	{
	if (vec->data)
		{
		for (unsigned long i = 0; i < vec->len; i++)
			drop(vec->data[i]);

		free_memory(vec->data, sizeof(value[vec->size]));

		vec->len = 0;
		vec->size = 0;
		vec->data = 0;
		}
	}

static void clear_vector(value f)
	{
	struct vector *vec = f->v_ptr;
	reset_vector(vec);
	free_memory(vec,sizeof(struct vector));
	}

// \item=(vec pos) # Use vec as a function of position.
static value type_vector(value f)
	{
	value key = arg(f->R);
	if (key->T == type_num)
		{
		struct vector *vec = f->L->v_ptr;
		unsigned long pos = key->v_double;
		if (pos < vec->len)
			f = hold(vec->data[pos]);
		else
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(key);
	return f;
	}

// \vec=vec_new # Create new vector.
static value type_vec_new(value f)
	{
	static struct value clear = {{.N=0}, {.clear=clear_vector}};
	struct vector *vec = new_memory(sizeof(struct vector));
	*vec = (struct vector){0};
	return V(type_vector,&clear,(value)vec);
	(void)f;
	}

// \n=(vec_len vec) # Get the number of items in the vector.
static value type_vec_len(value f)
	{
	value x = arg(f->R);
	if (x->T == type_vector)
		{
		struct vector *vec = x->v_ptr;
		f = Qnum(vec->len);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// vec_clear vec # Reset vec to empty.
static value type_vec_clear(value f)
	{
	value x = arg(f->R);
	if (x->T == type_vector)
		{
		struct vector *vec = x->v_ptr;
		reset_vector(vec);
		f = hold(QI);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}

// vec_get vec pos # Yield the item at position.
static value type_vec_get(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_vector)
		{
		value key = arg(f->R);
		if (key->T == type_num)
			{
			struct vector *vec = x->v_ptr;
			unsigned long pos = key->v_double;
			if (pos < vec->len)
				f = yield(hold(vec->data[pos]));
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(key);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

// vec_put vec pos item // Replace item at position.
static value type_vec_put(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value x = arg(f->L->L->R);
	if (x->T == type_vector)
		{
		value key = arg(f->L->R);
		if (key->T == type_num)
			{
			struct vector *vec = x->v_ptr;
			unsigned long pos = key->v_double;
			if (pos < vec->len)
				{
				value item = hold(f->R);
				drop(vec->data[pos]);
				vec->data[pos] = item;
				f = hold(QI);
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(key);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

// Calculate a size needed to hold at least count elements.
static unsigned long calc_size(unsigned long count)
	{
	unsigned long extra = count >> 1;
	const unsigned long min = 5;
	if (extra < min) extra = min;
	return count + extra;
	}

// Increase the size to hold at least one more element.
static void bump(struct vector *vec)
	{
	unsigned long o_size = vec->size;
	unsigned long n_size = calc_size(vec->len + 1);

	vec->data = resize_memory(vec->data,
		sizeof(value[o_size]),
		sizeof(value[n_size])
		);
	vec->size = n_size;
	}

// Shift left the items at [pos+1,n] so they are at [pos,n-1].
static void shift_l(value vec[], unsigned long pos, unsigned long n)
	{
	unsigned long i;
	for (i = pos; i < n; i++)
		vec[i] = vec[i+1];
	}

// Shift right the items at [pos,n-1] so they are at [pos+1,n].
static void shift_r(value vec[], unsigned long pos, unsigned long n)
	{
	unsigned long i;
	for (i = n; i > pos; i--)
		vec[i] = vec[i-1];
	}

// vec_ins vec pos item # Insert item at position.
static value type_vec_ins(value f)
	{
	if (f->L->L == 0) return keep(f);
	if (f->L->L->L == 0) return keep(f);
	{
	value x = arg(f->L->L->R);
	if (x->T == type_vector)
		{
		value key = arg(f->L->R);
		if (key->T == type_num)
			{
			struct vector *vec = x->v_ptr;
			unsigned long pos = key->v_double;

			if (pos <= vec->len)
				{
				if (vec->len >= vec->size)
					bump(vec);

				shift_r(vec->data, pos, vec->len);
				vec->data[pos] = hold(f->R);
				vec->len++;

				f = hold(QI);
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(key);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

// vec_del vec pos # Delete item at position.
static value type_vec_del(value f)
	{
	if (f->L->L == 0) return keep(f);
	{
	value x = arg(f->L->R);
	if (x->T == type_vector)
		{
		value key = arg(f->R);
		if (key->T == type_num)
			{
			struct vector *vec = x->v_ptr;
			unsigned long pos = key->v_double;
			if (pos < vec->len)
				{
				drop(vec->data[pos]);
				shift_l(vec->data, pos, vec->len);
				vec->len--;
				f = hold(QI);
				}
			else
				f = hold(Qvoid);
			}
		else
			f = hold(Qvoid);
		drop(key);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}

// \flag=(is_vec x) # True if x is a vector.
static value type_is_vec(value f)
	{
	return op_is_type(f,type_vector);
	}

value type_load_vector(value f)
	{
	define("vec_new",Q0(type_vec_new));
	define("vec_len",Q(type_vec_len));
	define("vec_clear",Q(type_vec_clear));
	define("vec_get",Q(type_vec_get));
	define("vec_put",Q(type_vec_put));
	define("vec_ins",Q(type_vec_ins));
	define("vec_del",Q(type_vec_del));
	define("is_vec",Q(type_is_vec));
	return hold(QI);
	(void)f;
	}
