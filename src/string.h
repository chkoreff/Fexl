struct atom_string
	{
	int N;
	int len;
	char data[];
	};

extern void hold_string(struct value *value);
extern void drop_string(struct value *value);
extern struct type type_string;
extern struct value *new_chars(const char *data, int len);
extern struct value *new_string(const char *data);
extern struct atom_string *evaluate_string(struct value *value);
