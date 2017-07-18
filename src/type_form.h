extern value type_form(value f);
extern value Qform(value exp, value label);
extern value form_exp(value f);
extern const char *form_label(value f);
extern value op_resolve(value resolve_name(const char *name), value f);
extern value type_define(value f);
extern value type_is_resolved(value f);
extern value type_evaluate(value f);
extern value type_evaluate_later(value f);
