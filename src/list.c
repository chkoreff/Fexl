#include "value.h"
#include "list.h"

/*
(item head tail) is the list consisting of the head element followed by the
tail list.  It can be defined as:
	\item = (\head\tail \end\item item head tail)

It obeys the equation:
	item h t f g = g h t
*/
struct value *type_item(struct value *f)
	{
	if (!f->L->L || !f->L->L->L || !f->L->L->L->L) return f;
	return A(A(f->R,f->L->L->L->R),f->L->L->R);
	}
