typedef void (*output)(const char *data, unsigned long len);
/* Use (void *) instead of (FILE *) to avoid include stdio.h everywhere. */
extern void *cur_out;
extern output putd; /* current output routine */
extern void put(const char *data);
extern void put_ch(char ch);
extern void put_long(long x);
extern void put_ulong(unsigned long x);
extern void put_double(double x);
extern void nl(void);
extern void put_to(void *out);
extern void put_to_error(void);
extern void flush(void);
