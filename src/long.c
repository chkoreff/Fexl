value type_long(value f)
	{
	return type_error();
	}

struct atom_long
	{
	void (*free)(struct atom_long *);
	long val;
	};

static void free_long(struct atom_long *x)
	{
	free_memory(x, sizeof(struct atom_long));
	}

value Qlong(long val)
	{
	struct atom_long *x = new_memory(sizeof(struct atom_long));
	x->free = free_long;
	x->val = val;
	return V(type_long,0,(value)x);
	}

long long_val(value f)
	{
	struct atom_long *x = (struct atom_long *)f->R;
	return x->val;
	}

value type_long_add1(value f)
	{
	long x = long_val(arg(type_long,&f->L->L));
	long y = long_val(arg(type_long,&f->R));
	return Qlong(x + y);
	}

value type_long_add(value f)
	{
	return V(type_long_add1,f->R,0);
	}

value type_long_sub1(value f)
	{
	long x = long_val(arg(type_long,&f->L->L));
	long y = long_val(arg(type_long,&f->R));
	return Qlong(x - y);
	}

value type_long_sub(value f)
	{
	return V(type_long_sub1,f->R,0);
	}

value type_long_mul1(value f)
	{
	long x = long_val(arg(type_long,&f->L->L));
	long y = long_val(arg(type_long,&f->R));
	return Qlong(x * y);
	}

value type_long_mul(value f)
	{
	return V(type_long_mul1,f->R,0);
	}

/* long_div returns 0 if you try to divide by 0. */
value type_long_div1(value f)
	{
	long x = long_val(arg(type_long,&f->L->L));
	long y = long_val(arg(type_long,&f->R));
	return Qlong(y ? x / y : 0);
	}

value type_long_div(value f)
	{
	return V(type_long_div1,f->R,0);
	}

/* (long_string x) Convert long to string. */
value type_long_string(value f)
	{
	long x = long_val(arg(type_long,&f->R));
	char buf[100]; /* being careful here */
	sprintf(buf, "%ld", x);
	return Qstring(buf);
	}

value resolve_long(const char *name)
	{
	if (strcmp(name,"add") == 0) return Q(type_long_add);
	if (strcmp(name,"sub") == 0) return Q(type_long_sub);
	if (strcmp(name,"mul") == 0) return Q(type_long_mul);
	if (strcmp(name,"div") == 0) return Q(type_long_div);
	if (strcmp(name,"string") == 0) return Q(type_long_string);
	return 0;
	}

#if 0
TODO more functions
long_double
long_string
long_char
long_cmp
order
#endif
