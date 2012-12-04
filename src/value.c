static value free_list = 0;

/* Put the value on the free list. */
static void recycle(value f)
	{
	f->N = (long)free_list;
	free_list = f;
	}

/* Increment the reference count. */
void hold(value f)
	{
	f->N++;
	}

/*
Decrement the reference count and put the value on the free list if it drops to
zero.  We are very strict about this.  It must be exactly zero, not negative.
A negative reference count indicates an imbalance in hold/drop calls, which is
sloppy programming, and in that case we want to report a memory leak error when
the program ends.
*/
void drop(value f)
	{
	if (--f->N == 0) recycle(f);
	}

/* Recycle the value if is reference count is zero.  Shorthand for doing a hold
and drop on the value. */
void check(value f)
	{
	if (f->N == 0) recycle(f);
	}

/* Evacuate the contents of a value retrieved from the free list. */
void clear(value v)
	{
	if (v->L)
		{
		drop(v->L);
		drop(v->R);
		}
	else if (v->R)
		{
		struct atom *x = (struct atom *)v->R;
		x->free(x);
		}
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	value v = free_list;

	if (v)
		{
		free_list = (value)v->N;
		clear(v);
		}
	else
		v = (value)new_memory(sizeof (struct value));

	v->N = 0;
	v->T = T;
	v->L = L;
	v->R = R;

	if (L)
		{
		hold(L);
		hold(R);
		}

	return v;
	}

/* Apply f to g with delayed evaluation. */
value A(value f, value g)
	{
	return V(0,f,g);
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return V(T,0,0);
	}

/* Evaluate the value at the given position, replacing it with its normal form
if possible within any limits on space and time. */
value eval(value *pos)
	{
	value f = *pos;
	while (f->T == 0)
		{
		value g = eval(&f->L)->T(f);
		if (g)
			{
			hold(g);
			drop(f);
			f = g;
			}
		else
			f->T = f->L->T;
		}
	*pos = f;
	return f;
	}

value type_error(void)
	{
	die("You used a data type incorrectly.");
	return 0;
	}

/* Evaluate an argument and assert that its type is T. */
value arg(type T, value *pos)
	{
	value x = eval(pos);
	if (x->T != T) type_error();
	return x;
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));

	end_memory();
	}
