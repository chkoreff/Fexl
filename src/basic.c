/* C x y = x */
value type_C(value f)
	{
	if (!f->L->L) return 0;
	return f->L->R;
	}

/* S x y z = x z (y z) */
value type_S(value f)
	{
	if (!f->L->L || !f->L->L->L) return 0;
	return A(A(f->L->L->R,f->R),A(f->L->R,f->R));
	}

/* I x = x */
value type_I(value f)
	{
	return f->R;
	}

/* F x = I.  In other words, F x y = y. */
value type_F(value f)
	{
	return I;
	}

/* R x y z = x (y z) */
value type_R(value f)
	{
	if (!f->L->L || !f->L->L->L) return 0;
	return A(f->L->L->R,A(f->L->R,f->R));
	}

/* L x y z = x z y */
value type_L(value f)
	{
	if (!f->L->L || !f->L->L->L) return 0;
	return A(A(f->L->L->R,f->R),f->L->R);
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	return A(f->R,A(f->L,f->R));
	}

/* item x y A B = B x y */
value type_item(value f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;
	return A(A(f->R,f->L->L->L->R),f->L->L->R);
	}

/* Return the list with head h and tail t, which is [h;t] in list notation. */
value item(value h, value t)
	{
	return A(A(Qitem,h),t);
	}

/* Return T or F based on the flag. */
value boolean(int flag)
	{
	return Q(flag ? type_C : type_F);
	}

/* Return [x] if x is non-zero, otherwise []. */
value maybe(value x)
	{
	value C = Q(type_C);
	return x ? item(x,C) : C;
	}

value C;
value S;
value I;
value R;
value L;
value Y;
value Qitem;

void beg_basic(void)
	{
	C = Q(type_C); hold(C);
	S = Q(type_S); hold(S);
	I = Q(type_I); hold(I);
	R = Q(type_R); hold(R);
	L = Q(type_L); hold(L);
	Y = Q(type_Y); hold(Y);
	Qitem = Q(type_item); hold(Qitem);
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
	}
