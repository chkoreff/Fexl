struct atom_long
	{
	int N;
	long data;
	};

extern struct type type_long;
extern struct value *new_long(long x);
extern long evaluate_long(struct value *value);
