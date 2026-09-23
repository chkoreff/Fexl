struct istr
	{
	unsigned long pos;
	value str;
	};

extern value type_istr(value f);
extern int sgetc(struct istr *in);
extern value type_load_istr(value f);
