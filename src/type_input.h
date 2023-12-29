typedef int (*input)(void *source);
extern value op_getc(value fun, value f, type t, input get);
extern value op_get(value fun, value f, type t, input get);
