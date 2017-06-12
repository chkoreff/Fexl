#include <str.h>
#include <value.h>

#include <basic.h>
#include <memory.h>
#include <type_form.h>
#include <type_str.h>

static void form_free(form f)
	{
	drop(f->label);
	drop(f->exp);
	free_memory(f,sizeof(struct form));
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
	return (form)f->R;
	}

value form_label(value f)
	{
	return get_form(f)->label;
	}

value form_exp(value f)
	{
	return get_form(f)->exp;
	}
