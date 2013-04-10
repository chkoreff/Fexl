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

/* Replace value f with the given components, or make a new value if f==0. */
value replace(value f, type T, value L, value R)
	{
	if (f == 0)
		{
		f = free_list;

		if (f)
			free_list = (value)f->N;
		else
			{
			f = (value)new_memory(sizeof (struct value));
			f->L = 0;
			f->R = 0;
			}

		f->N = 0;
		}

	if (L) hold(L);
	if (R) hold(R);

	if (f->L)
		{
		/* Clear pair. */
		drop(f->L);
		drop(f->R);
		}
	else if (f->R && --f->R->N == 0)
		{
		/* Clear data. */
		if (f->R->T)
			f->R->T(f->R); /* Call the clear routine for the data. */

		f->R->L = 0;
		f->R->R = 0;
		recycle(f->R);
		}

	f->T = T;
	f->L = L;
	f->R = R;

	return f;
	}

/* Return a value of type T with the given left and right side. */
value V(type T, value L, value R)
	{
	return replace(0,T,L,R);
	}

/* Create a combinator of type T.  Shorthand for "quote". */
value Q(type T)
	{
	return V(T,0,0);
	}

/* The reduction routine for function application. */
value type_apply(value f)
	{
	value g = eval(f->L);
	if (f->L == g)
		{
		f->T = f->L->T;
		return f;
		}
	else
		return V(g->T,g,f->R);
	}

/* Return a value which is f applied to g. */
value A(value f, value g)
	{
	return V(type_apply,f,g);
	}

/* Replace value f with value g. */
value replace_value(value f, value g)
	{
	return replace(f, g->T, g->L, g->R);
	}

/* Replace value f with A(L,R). */
value replace_apply(value f, value L, value R)
	{
	return replace(f, type_apply, L, R);
	}

/* Evaluate the value, returning its normal form if possible within any limits
on space and time. */
value eval(value f)
	{
	hold(f);
	while (1)
		{
		value g = f->T(f);
		if (g == 0)
			{
			f->N--;
			return f;
			}
		if (g != f)
			{
			hold(g);
			drop(f);
			f = g;
			}
		}
	}

/* Evaluate an argument and assert that it is of type T. */
value arg(type T, value f)
	{
	if (f->T == T) return f;
	f = eval(f);
	if (f->T == T) return f;
	die("You used a data type incorrectly.");
	return 0;
	}

/* Clear the free list and end the memory arena. */
void end_value(void)
	{
	while (free_list)
		free_memory(Q(0), sizeof(struct value));

	end_memory();
	}
