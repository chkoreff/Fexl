struct istr
	{
	unsigned long pos;
	value str;
	};

extern int sgetc(struct istr *in);
extern value Qistr(value x);
extern struct istr *get_istr(value x);
extern value type_istr(value f);
extern value type_readstr(value f);
extern value type_sgetc(value f);
extern value type_sget(value f);
