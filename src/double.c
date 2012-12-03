value type_double(value f)
	{
	return type_error();
	}

struct atom_double
	{
	void (*free)(struct atom_double *);
	double val;
	};

static void free_double(struct atom_double *x)
	{
	free_memory(x, sizeof(struct atom_double));
	}

value Qdouble(double val)
	{
	struct atom_double *x = new_memory(sizeof(struct atom_double));
	x->free = free_double;
	x->val = val;
	return V(type_double,0,(value)x);
	}

double double_val(value f)
	{
	struct atom_double *x = (struct atom_double *)f->R;
	return x->val;
	}

value type_double_add1(value f)
	{
	double x = double_val(arg(type_double,&f->L->L));
	double y = double_val(arg(type_double,&f->R));
	return Qdouble(x + y);
	}

value type_double_add(value f)
	{
	return V(type_double_add1,f->R,0);
	}

value type_double_sub1(value f)
	{
	double x = double_val(arg(type_double,&f->L->L));
	double y = double_val(arg(type_double,&f->R));
	return Qdouble(x - y);
	}

value type_double_sub(value f)
	{
	return V(type_double_sub1,f->R,0);
	}

value type_double_mul1(value f)
	{
	double x = double_val(arg(type_double,&f->L->L));
	double y = double_val(arg(type_double,&f->R));
	return Qdouble(x * y);
	}

value type_double_mul(value f)
	{
	return V(type_double_mul1,f->R,0);
	}

/*
Note that dividing by zero is no problem here. If you divide a non-zero by
zero, it yields inf (infinity). If you divide zero by zero, it yields -nan
(not a number).
*/
value type_double_div1(value f)
	{
	double x = double_val(arg(type_double,&f->L->L));
	double y = double_val(arg(type_double,&f->R));
	return Qdouble(x / y);
	}

value type_double_div(value f)
	{
	return V(type_double_div1,f->R,0);
	}

/* (double_string x) Convert double to string. */
value type_double_string(value f)
	{
	double x = double_val(arg(type_double,&f->R));
	char buf[100]; /* being careful here */
	/* We show 15 digits because that's what Perl does. */
	sprintf(buf, "%.15g", x);
	return Qstring(buf);
	}

value resolve_double(const char *name)
	{
	if (strcmp(name,"add") == 0) return Q(type_double_add);
	if (strcmp(name,"sub") == 0) return Q(type_double_sub);
	if (strcmp(name,"mul") == 0) return Q(type_double_mul);
	if (strcmp(name,"div") == 0) return Q(type_double_div);
	if (strcmp(name,"string") == 0) return Q(type_double_string);
	return 0;
	}

#if 0
TODO more functions
double_long
double_string
double_cmp
#endif
