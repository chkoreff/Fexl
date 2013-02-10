value type_double(value f)
	{
	return type_C(f);
	}

/*
A double is guaranteed to fit in 64 bits.  The standard says:

  Type        Range: Min   Max           Size (Bits)
  double      2.225e-308   1.7977e+308   64

(C Language Reference Manual, document 007-0701-150, Appendix A, Section F.3.6
"Floating Point")

We know that the L and R pointers are either 32 or 64 bits each, depending on
the machine, so together they can definitely hold the 64 bits of a double.
*/
value replace_double(value f, double val)
	{
	value x = Q(0);
	double *p = (double *)(&x->L);
	*p = val;
	return replace(f, type_double, 0, x);
	}

value Qdouble(double val)
	{
	return replace_double(0,val);
	}

/*
Note that we cannot retrieve a double value directly like this:
	double x = *( (double *)&f->R->L );

That yields an error:
	dereferencing type-punned pointer will break strict-aliasing rules
	[-Werror=strict-aliasing]

Instead, we must first get a pointer to double, then dereference the pointer.
*/
double double_val(value f)
	{
	double *p = (double *)&f->R->L;
	return *p;
	}

value type_double_add(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_double,f->L->R);
	value y = arg(type_double,f->R);

	double vx = double_val(x);
	double vy = double_val(y);
	double vz = vx + vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_double(f,vz);
	}

value type_double_sub(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_double,f->L->R);
	value y = arg(type_double,f->R);

	double vx = double_val(x);
	double vy = double_val(y);
	double vz = vx - vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_double(f,vz);
	}

value type_double_mul(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_double,f->L->R);
	value y = arg(type_double,f->R);

	double vx = double_val(x);
	double vy = double_val(y);
	double vz = vx * vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_double(f,vz);
	}

/*
Note that dividing by zero is no problem here. If you divide a non-zero by
zero, it yields inf (infinity). If you divide zero by zero, it yields -nan
(not a number).
*/
value type_double_div(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = arg(type_double,f->L->R);
	value y = arg(type_double,f->R);

	double vx = double_val(x);
	double vy = double_val(y);
	double vz = vx / vy;

	if (x != f->L->R || y != f->R)
		{
		check(x);
		check(y);
		f = 0;
		}
	return replace_double(f,vz);
	}

/* (double_string x) Convert double to string. */
value type_double_string(value f)
	{
	if (!f->L) return 0;

	value x = arg(type_double,f->R);
	double vx = double_val(x);
	char data[100]; /* being careful here */
	/* We show 15 digits because that's what Perl does. */
	sprintf(data, "%.15g", vx);

	if (x != f->R)
		{
		check(x);
		f = 0;
		}
	return replace_string(f,data);
	}

value type_is_double(value f)
	{
	return replace_is_type(f, type_double);
	}

static value resolve_double_prefix(const char *name)
	{
	if (strcmp(name,"add") == 0) return Q(type_double_add);
	if (strcmp(name,"sub") == 0) return Q(type_double_sub);
	if (strcmp(name,"mul") == 0) return Q(type_double_mul);
	if (strcmp(name,"div") == 0) return Q(type_double_div);
	if (strcmp(name,"string") == 0) return Q(type_double_string);
	return 0;
	}

/* Convert string to double and return true if successful. */
static int string_double(const char *beg, double *num)
	{
	char *end;
	*num = strtod(beg, &end);
	return *beg != '\0' && *end == '\0';
	}

value resolve_double(const char *name)
	{
	/* Floating point number (double) */
	{
	double num;
	if (string_double(name,&num)) return Qdouble(num);
	}

	if (strncmp(name,"double_",7) == 0) return resolve_double_prefix(name+7);
	if (strcmp(name,"is_double") == 0) return Q(type_is_double);
	return 0;
	}

/* TODO more functions
double_long
double_cmp
*/
