typedef void (*output)(const char *data, unsigned long len);
extern output putd; /* current output function */

extern void put(const char *data);
extern void put_ch(char ch);
extern void put_long(long x);
extern void put_ulong(unsigned long x);
extern void put_double(double x);
extern void nl(void);
extern void put_to_error(void);
