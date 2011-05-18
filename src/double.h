struct atom_double
	{
	int N;
	double data;
	};

extern struct type type_double;
extern struct value *new_double(double x);
extern double evaluate_double(struct value *value);
