value type_long(value f)
	{
	return type_C(f);
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

value type_long_add(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);

	long vx = long_val(x);
	long vy = long_val(y);
	long vz = vx + vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,vz);
	}

value type_long_sub(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);

	long vx = long_val(x);
	long vy = long_val(y);
	long vz = vx - vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,vz);
	}

value type_long_mul(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);

	long vx = long_val(x);
	long vy = long_val(y);
	long vz = vx * vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,vz);
	}

/* long_div returns 0 if you try to divide by 0. */
value type_long_div(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);

	long vx = long_val(x);
	long vy = long_val(y);
	long vz = vy ? vx / vy : 0;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,vz);
	}

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

value type_long_cmp(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_long,f->L->R);
	value y = arg(type_long,f->R);

	long vx = long_val(x);
	long vy = long_val(y);
	long vz = vx < vy ? -1 : vx > vy ? 1 : 0;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_long(f,vz);
	}

value type_is_long(value f)
	{
	return replace_is_type(f, type_long);
	}

/* Convert string to long and return true if successful. */
static int string_long(const char *beg, long *num)
	{
	char *end;
	*num = strtol(beg, &end, 10);
	return *beg != '\0' && *end == '\0';
	}

/* (order x lt eq gt) compares x and 0 and returns lt, eq, or gt. */
value type_order(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;

	value x = arg(type_long,f->L->L->L->R);
	long vx = long_val(x);
	value result = vx < 0 ? f->L->L->R : vx > 0 ? f->R : f->L->R;

	if (x != f->L->L->L->R)
		{
		check(x);
		f = 0;
		}
	return replace_value(f,result);
	}

static value resolve_long_prefix(const char *name)
	{
	if (strcmp(name,"add") == 0) return Q(type_long_add);
	if (strcmp(name,"sub") == 0) return Q(type_long_sub);
	if (strcmp(name,"mul") == 0) return Q(type_long_mul);
	if (strcmp(name,"div") == 0) return Q(type_long_div);
	if (strcmp(name,"cmp") == 0) return Q(type_long_cmp);
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
	if (strcmp(name,"order") == 0) return Q(type_order);
	return 0;
	}

#if 0
LATER more functions
long_double
long_string
long_char
#endif
