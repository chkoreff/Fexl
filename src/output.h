typedef void (*target)(const char *data, unsigned long len);

extern target beg_error(void);
extern void end_error(target old);
extern target putd;

extern void put(const char *data);
extern void nl(void);
extern void put_long(long x);
extern void put_ulong(unsigned long x);
