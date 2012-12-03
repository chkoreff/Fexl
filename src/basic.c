/* C x y = x */
value type_C1(value f)
	{
	return f->L->L;
	}

value type_C(value f)
	{
	return V(type_C1,f->R,0);
	}

/* S x y z = x z (y z) */
value type_S2(value f)
	{
	value x = f->L->L;
	value y = f->L->R;
	value z = f->R;

	return A(A(x,z),A(y,z));
	}

value type_S1(value f)
	{
	return V(type_S2,f->L->L,f->R);
	}

value type_S(value f)
	{
	return V(type_S1,f->R,0);
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
value type_R2(value f)
	{
	value x = f->L->L;
	value y = f->L->R;
	value z = f->R;

	return A(x,A(y,z));
	}

value type_R1(value f)
	{
	return V(type_R2,f->L->L,f->R);
	}

value type_R(value f)
	{
	return V(type_R1,f->R,0);
	}

/* L x y z = x z y */
value type_L2(value f)
	{
	value x = f->L->L;
	value y = f->L->R;
	value z = f->R;

	return A(A(x,z),y);
	}

value type_L1(value f)
	{
	return V(type_L2,f->L->L,f->R);
	}

value type_L(value f)
	{
	return V(type_L1,f->R,0);
	}

/* Y x = x (Y x) */
value type_Y(value f)
	{
	return A(f->R,A(f->L,f->R));
	}

/* pair x y B = B x y */
value type_pair2(value f)
	{
	return A(A(f->R,f->L->L),f->L->R);
	}

/* item x y A B = B x y */
value type_item2(value f)
	{
	return V(type_pair2,f->L->L,f->L->R);
	}

value type_item1(value f)
	{
	return V(type_item2, f->L->L, f->R);
	}

value type_item(value f)
	{
	return V(type_item1, f->R, 0);
	}

/* Return the list with head h and tail t, which is [h;t] in list notation. */
value item(value h, value t)
	{
	return V(type_item2,h,t);
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
