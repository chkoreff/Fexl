struct str_input
	{
	input getd;
	string source;
	unsigned long pos;
	};

extern void get_from_string(string x, struct str_input *save);
extern void restore_str_input(struct str_input *save);
