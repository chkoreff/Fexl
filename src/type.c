#include "value.h"
#include "long.h"

/* TODO I'm implementing a data type "type" (type_type) to represent handles to
built-in type routines.  That will let you check and compare types in Fexl, so
you can write things like "show" in Fexl itself.
*/

value type_type(value f) { return f; }

/* Make a new value of type "type".  */
value Qtype(type T)
	{
	return Qlong((long)T);
	}

type get_type(value f)
	{
	return (type)f->L->R;
	}
