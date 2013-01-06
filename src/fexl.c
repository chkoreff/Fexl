/* Command line arguments */
int argc;
char **argv;

int main(int _argc, char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	beg_basic();

	value f = parse_file(argc > 1 ? argv[1] : "", Q(type_resolve), 1);
	value g = eval(f);
	check(g);

	end_basic();
	end_value();
	return 0;
	}
