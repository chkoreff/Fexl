value type_long(value f) { return type_data(f); }

value replace_long(value f, long val)
	{
	value x = Q(0);
	x->L = (value)val;
	return replace(f, type_long, 0, x);
	}

value Qlong(long val)
	{
	return replace_long(0,val);
	}

long long_val(value f)
	{
	return (long)f->R->L;
	}

static value op2(value f, long op(long,long))
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);
	long z = op(long_val(x),long_val(y));

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,z);
	}

static long op_add(long x, long y) { return x + y; }
static long op_sub(long x, long y) { return x - y; }
static long op_mul(long x, long y) { return x * y; }
/* op_div returns 0 if you try to divide by 0. */
static long op_div(long x, long y) { return y ? x / y : 0; }

static value type_long_add(value f) { return op2(f,op_add); }
static value type_long_sub(value f) { return op2(f,op_sub); }
static value type_long_mul(value f) { return op2(f,op_mul); }
static value type_long_div(value f) { return op2(f,op_div); }

/* (long_string x) Convert long to string. */
static value type_long_string(value f)
	{
	if (!f->L) return 0;

	value x = arg(type_long,f->R);
	long vx = long_val(x);
	char data[100]; /* being careful here */
	sprintf(data, "%ld", vx);

	if (x != f->R)
		{
		check(x);
		f = 0;
		}
	return replace_string(f,data);
	}

/* Convert string to long and return true if successful. */
static int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

static int op_cmp(long x, long y)
	{
	return x < y ? -1 : x > y ? 1 : 0;
	}

static int long_cmp(value x, value y)
	{
	return op_cmp(long_val(x),long_val(y));
	}

/* (long_compare x y lt eq gt) */
static value type_long_compare(value f)
	{
	return op_compare(f,type_long,long_cmp);
	}

static value type_is_long(value f)
	{
	return op_is_atom(f, type_long);
	}

static value resolve_long_prefix(const char *name)
	{
	if (strcmp(name,"add") == 0) return Q(type_long_add);
	if (strcmp(name,"sub") == 0) return Q(type_long_sub);
	if (strcmp(name,"mul") == 0) return Q(type_long_mul);
	if (strcmp(name,"div") == 0) return Q(type_long_div);
	if (strcmp(name,"compare") == 0) return Q(type_long_compare);
	if (strcmp(name,"string") == 0) return Q(type_long_string);
	return 0;
	}

value resolve_long(const char *name)
	{
	/* Integer number (long) */
	{
	long num;
	if (string_long(name,&num)) return Qlong(num);
	}

	if (strncmp(name,"long_",5) == 0) return resolve_long_prefix(name+5);
	if (strcmp(name,"is_long") == 0) return Q(type_is_long);
	return 0;
	}

#if 0
LATER more functions
long_double
long_string
long_char
#endif
