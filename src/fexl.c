/* Command line arguments */
int argc;
char **argv;

int main(int _argc, char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	beg_basic();

	const char *name = argc > 1 ? argv[1] : "";
	value f = parse_file(name, standard_context());
	value g = eval(f);
	check(g);

	end_basic();
	end_value();
	return 0;
	}
