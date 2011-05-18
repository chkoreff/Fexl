#include <string.h>
#include "value.h"
#include "abstract.h"
#include "apply.h"
#include "C.h"
#include "S.h"
#include "I.h"
#include "L.h"
#include "R.h"
#include "Y.h"
#include "string.h"
#include "var.h"

int sym_eq(struct value *x, struct value *y)
	{
	struct atom_string *atom_x = (struct atom_string *)x->R;
	struct atom_string *atom_y = (struct atom_string *)y->R;

	return atom_x->len == atom_y->len &&
		memcmp(atom_x->data, atom_y->data, atom_x->len) == 0;
	}

static struct value *C = &value_C;
static struct value *I = &value_I;
static struct value *L = &value_L;
static struct value *R = &value_R;
static struct value *S = &value_S;

int match_C(struct value *value)
	{
	return !value->L && value->T == &type_C;
	}

int match_I(struct value *value)
	{
	return !value->L && value->T == &type_I;
	}

int match_S(struct value *value)
	{
	return !value->L && value->T == &type_S;
	}

int match_Y(struct value *value)
	{
	return !value->L && value->T == &type_Y;
	}

int match_Cx(struct value *value)
	{
	return value->L && match_C(value->L);
	}

int match_Sx(struct value *value)
	{
	return value->L && match_S(value->L);
	}

int match_S_Cx(struct value *value)
	{
	return value->L && match_S(value->L) && match_Cx(value->R);
	}

int match_S_Cx_y(struct value *value)
	{
	return value->L && match_S_Cx(value->L);
	}

int match_S_x_Cy(struct value *value)
	{
	return value->L && match_Sx(value->L) && match_Cx(value->R);
	}

int match_S_Cx_I(struct value *value)
	{
	return match_S_Cx_y(value) && match_I(value->R);
	}

int match_S_Cx_Cy(struct value *value)
	{
	return match_S_Cx_y(value) && match_Cx(value->R);
	}

int match_Y_Cx(struct value *value)
	{
	return value->L && match_Y(value->L) && match_Cx(value->R);
	}

void optimize(struct value *value)
	{
	if (match_S_Cx_I(value))
		{
		/* S (C x) I = x */
		replace(value, value->L->R->R);
		return;
		}

	if (match_S_Cx_Cy(value))
		{
		/* S (C x) (C y) = C (x y) */
		replace(value, apply(C,apply(value->L->R->R,value->R->R)));
		return;
		}

	if (match_S_Cx_y(value))
		{
		/* S (C x) y = R x y */
		replace(value, apply(apply(R,value->L->R->R),value->R));
		return;
		}

	if (match_S_x_Cy(value))
		{
		/* S x (C y) = L x y */
		replace(value, apply(apply(L,value->L->R),value->R->R));
		return;
		}

	if (match_Y_Cx(value))
		{
		/* Y (C x) = x */
		replace(value, value->R->R);
		return;
		}
	}

struct value *abstract(struct value *sym, struct value *body)
	{
	if (!body->L)
		{
		if (body->T == &type_var && sym_eq(sym,body)) return I;
		return apply(C,body);
		}

	struct value *value =
		apply(apply(S,abstract(sym,body->L)),abstract(sym,body->R));

	optimize(value);
	return value;
	}
