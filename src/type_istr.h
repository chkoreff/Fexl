struct istr
	{
	unsigned long pos;
	value str;
	};

extern value Qistr(value x);
extern int sgetc(struct istr *in);
extern struct istr *get_istr(value x);
extern value type_istr(value fun, value f);
extern value type_readstr(value fun, value f);
extern value type_sgetc(value fun, value f);
extern value type_sget(value fun, value f);
extern value type_slook(value fun, value f);
