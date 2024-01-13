struct istr
	{
	unsigned long pos;
	value str;
	};

extern value Qistr(value x);
extern int sgetc(struct istr *in);
extern value type_istr(value f);
extern value type_readstr(value f);
extern value type_sgetc(value f);
extern value type_sget(value f);
extern value type_slook(value f);
