struct file_input
	{
	input getd;
	FILE *source;
	};

extern void get_from_file(const char *name, struct file_input *save);
void restore_file_input(struct file_input *save);
