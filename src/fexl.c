/* Command line arguments */
int argc;
char **argv;

int main(int _argc, char *_argv[])
	{
	argc = _argc;
	argv = _argv;

	beg_basic();

	#if 1
	extern void use_safe_limits(void);
	use_safe_limits(); /*LATER*/
	#endif

	value exp = parse_file(argc > 1 ? argv[1] : "", lib("standard"), 1);

	hold(exp);
	eval(&exp);
	drop(exp);

	end_basic();
	end_value();
	return 0;
	}
