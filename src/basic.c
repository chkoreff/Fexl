/* C x y = x */
value type_C(value f)
	{
	if (!f->L || !f->L->L) return 0;
	return replace_value(f, f->L->R);
	}

/* S x y z = x z (y z) */
value type_S(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return replace_apply(f, A(f->L->L->R,f->R), A(f->L->R,f->R));
	}

/* I x = x */
value type_I(value f)
	{
	if (!f->L) return 0;
	return replace_value(f, f->R);
	}

/* F x = I.  In other words, F x y = y. */
value type_F(value f)
	{
	if (!f->L) return 0;
	return replace_value(f, I);
	}

/* R x y z = x (y z) */
value type_R(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return replace_apply(f, f->L->L->R, A(f->L->R,f->R));
	}

/* L x y z = x z y */
value type_L(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L) return 0;
	return replace_apply(f, A(f->L->L->R,f->R), f->L->R);
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	if (!f->L) return 0;
	return replace_apply(f, f->R, A(f->L,f->R));
	}

/* item x y A B = B x y */
value type_item(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	return replace_apply(f, A(f->R,f->L->L->L->R), f->L->L->R);
	}

/* (query x y) = y x, except x is evaluated first. */
value type_query(value f)
	{
	if (!f->L || !f->L->L) return 0;

	value x = eval(f->L->R);
	value y = f->R;

	if (x != f->L->R)
		f = 0;
	return replace_apply(f, y, x);
	}

/* (return x) = x, except x is not evaluated right away.
This is useful for returning a function without evaluating it.  For example,
you may have a function which creates another function, and you want to return
that function without actually calling it.  This is particularly important for
functions which have side effects, otherwise we wouldn't really need this.
*/
value type_return(value f)
	{
	if (!f->L) return 0;
	replace_value(f, f->R);
	return 0;
	}

/* Become T or F based on the flag. */
value replace_boolean(value f, int flag)
	{
	return replace(f, flag ? type_C : type_F, 0, 0);
	}

/* Become [x] if x is non-zero, otherwise []. */
value replace_maybe(value f, value x) /*TODO*/
	{
	if (x)
		return replace_apply(f, A(Qitem,x), C);
	else
		return replace(f, type_C, 0, 0);
	}

/* Become T or F based on whether f->R is of type t. */
value replace_is_type(value f, type t) /*TODO*/
	{
	if (!f->L) return 0;
	return replace_boolean(f, f->R->T == t);
	}

value type_is_end(value f) /*TODO*/
	{
	return replace_is_type(f, type_C);
	}

value C;
value S;
value I;
value R;
value L;
value Y;
value Qitem;
value Qquery;

/* Return the list with head h and tail t, which is [h;t] in list notation. */
value item(value h, value t)
	{
	return A(A(Qitem,h),t);
	}

/* We use this to return an arbitrary tuple of values.  For example, to return
the triple (\: : x y z), we would say:
	return yield(yield(yield(I,x),y),z);
*/
value yield(value f, value g)
	{
	return A(A(L,f),g);
	}

/* (compare x y lt eq gt) compares x and y and returns lt, eq, or gt. */
value replace_compare(value f, type t, int cmp(value,value))
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L
		|| !f->L->L->L->L->L) return 0;

	value x = arg(t,f->L->L->L->L->R);
	value y = arg(t,f->L->L->L->R);
	int n = cmp(x,y);

	value result = n < 0 ? f->L->L->R : n > 0 ? f->R : f->L->R;

	if (x != f->L->L->L->L->R || y != f->L->L->L->R)
		{
		check(x);
		check(y);
		f = 0;
		}

	return replace_value(f, result);
	}

value resolve_basic(const char *name)
	{
	if (strcmp(name,"T") == 0) return C;
	if (strcmp(name,"F") == 0) return Q(type_F);
	if (strcmp(name,"I") == 0) return I;

	/* Define "@" as the fixpoint operator because it suggests making a
	"label" for looping. */
	if (strcmp(name,"@") == 0) return Y;

	if (strcmp(name,"C") == 0) return C;
	if (strcmp(name,"S") == 0) return S;
	if (strcmp(name,"Y") == 0) return Y;
	if (strcmp(name,"R") == 0) return R;
	if (strcmp(name,"L") == 0) return L;
	if (strcmp(name,"end") == 0) return C;
	if (strcmp(name,"item") == 0) return Qitem;
	if (strcmp(name,"?") == 0) return Qquery;
	if (strcmp(name,"query") == 0) return Qquery;
	if (strcmp(name,"return") == 0) return Q(type_return);
	if (strcmp(name,"is_end") == 0) return Q(type_is_end);
	return 0;
	}

void beg_basic(void)
	{
	C = Q(type_C); hold(C);
	S = Q(type_S); hold(S);
	I = Q(type_I); hold(I);
	R = Q(type_R); hold(R);
	L = Q(type_L); hold(L);
	Y = Q(type_Y); hold(Y);
	Qitem = Q(type_item); hold(Qitem);
	Qquery = Q(type_query); hold(Qquery);
	}

void end_basic(void)
	{
	drop(C);
	drop(S);
	drop(I);
	drop(R);
	drop(L);
	drop(Y);
	drop(Qitem);
	drop(Qquery);
	}
