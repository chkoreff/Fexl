#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_form.h>
#include <type_str.h>

static void form_free(form form)
	{
	drop(form->label);
	drop(form->exp);
	free_memory(form,sizeof(struct form));
	}

value type_form(value f)
	{
	if (f->N == 0)
		{
		form_free(get_form(f));
		return 0;
		}
	return type_void(f);
	}

value Qform(value label, value exp)
	{
	form x = new_memory(sizeof(struct form));
	x->label = label;
	x->exp = exp;
	return D(type_form,x);
	}

form get_form(value f)
	{
	return (form)data(f);
	}