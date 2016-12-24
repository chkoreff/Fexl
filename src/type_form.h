typedef struct form *form;
struct form
	{
	value label;
	value exp;
	};

extern value type_form(value f);
extern value Qform(value label, value exp);
extern form get_form(value f);
