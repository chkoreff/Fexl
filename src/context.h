extern value type_form(value fun, value arg);
extern void put_error_location(unsigned long line, const char *label);
extern value type_restrict(value fun, value arg);
extern value cx_std;
extern void beg_context(void);
extern void end_context(void);
