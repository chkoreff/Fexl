value type_long(value f)
	{
	return type_C(f); /*TODO*/
	}

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

static value replace_long2(value f, long op(long,long))
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

static long long_add(long x, long y) { return x + y; }
static long long_sub(long x, long y) { return x - y; }
static long long_mul(long x, long y) { return x * y; }
/* long_div returns 0 if you try to divide by 0. */
static long long_div(long x, long y) { return y ? x / y : 0; }

value type_long_add(value f) { return replace_long2(f,long_add); }
value type_long_sub(value f) { return replace_long2(f,long_sub); }
value type_long_mul(value f) { return replace_long2(f,long_mul); }
value type_long_div(value f) { return replace_long2(f,long_div); }

/* (long_string x) Convert long to string. */
value type_long_string(value f)
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
value type_long_compare(value f)
	{
	return replace_compare(f,type_long,long_cmp);
	}

value type_is_long(value f)
	{
	return replace_is_type(f, type_long);
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
