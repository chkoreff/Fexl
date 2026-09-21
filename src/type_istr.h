struct istr
	{
	unsigned long pos;
	value str;
	};

extern value type_istr(value f);
extern int sgetc(struct istr *in);
extern void load_istr(void);
