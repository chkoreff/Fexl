typedef int (*input)(void *source);
extern value op_getc(value f, type t, input get);
extern value op_get(value f, type t, input get);
