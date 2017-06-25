#include <value.h>

#include <basic.h>
#include <type_form.h>

value type_form(value f)
	{
	if (f->N == 0)
		{
		drop(f->R);
		return 0;
		}
	return type_void(f);
	}

value Qform(value exp)
	{
	return D(type_form,exp);
	}

value form_exp(value f)
	{
	return f->R;
	}
