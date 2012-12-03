/* This is the standard context which supplies definitions for any symbols used
in a Fexl program.  It is not linked into the fexl executable, but is put into
lib/fexl/standard.so and loaded dynamically.  From there, it can load yet other
shared libraries in lib/fexl as needed, and can even load code written in Fexl
in the share/fexl directory as well.  In this way, we can extend the standard
context indefinitely without changing or growing the fexl executable itself. */

value resolve_name(const char *name)
	{
	/* Integer number (long) */
	{
	long num;
	if (string_long(name,&num)) return Qlong(num);
	}

	/* Floating point number (double) */
	{
	double num;
	if (string_double(name,&num)) return Qdouble(num);
	}

	if (strcmp(name,"T") == 0) return C;
	if (strcmp(name,"F") == 0) return Q(type_F);

	if (strcmp(name,".") == 0) return Q(type_string_append);
	if (strncmp(name,"long_",5) == 0) return resolve_long(name+5);
	if (strncmp(name,"double_",7) == 0) return resolve_double(name+7);
	if (strncmp(name,"string_",7) == 0) return resolve_string(name+7);
	if (strncmp(name,"std",3) == 0) return resolve_std_file(name+3);
	if (strncmp(name,"is_",3) == 0) return resolve_is(name+3);
	if (strncmp(name,"var_",4) == 0) return resolve_var(name+4);

	if (strcmp(name,"?") == 0) return Q(type_query);
	if (strcmp(name,"exit") == 0) return Q(type_exit);
	if (strcmp(name,"fwrite") == 0) return Q(type_fwrite);
	if (strcmp(name,"fopen") == 0) return Q(type_fopen);
	if (strcmp(name,"putchar") == 0) return Q(type_putchar);

	if (strcmp(name,"I") == 0) return I;
	if (strcmp(name,"C") == 0) return C;
	if (strcmp(name,"S") == 0) return S;
	if (strcmp(name,"Y") == 0) return Y;
	if (strcmp(name,"R") == 0) return R;
	if (strcmp(name,"L") == 0) return L;
	if (strcmp(name,"end") == 0) return C;
	if (strcmp(name,"item") == 0) return Qitem;

	if (strcmp(name,"readlink") == 0) return Q(type_readlink);
	if (strcmp(name,"base_path") == 0) return Q(type_base_path);

	if (strncmp(name,"RLIMIT_",7) == 0) return resolve_RLIMIT(name+7);
	#if 0
	if (strcmp(name,"setrlimit") == 0) return Q(type_setrlimit);
	#endif

	#if 0
	/*TODO more functions */
	if (strcmp(name,"argc") == 0) return Q(type_argc);
	if (strcmp(name,"argv") == 0) return Q(type_argv);
	if (strcmp(name,"getenv") == 0) return Q(type_getenv);
	if (strcmp(name,"setenv") == 0) return Q(type_setenv);
	if (strcmp(name,"parse") == 0) return Q(type_parse);
	if (strcmp(name,"resolve") == 0) return Q(type_resolve);
	if (strcmp(name,"source") == 0) return Qsource();
	#endif

	return 0;
	}

/* (resolve name) returns [def] if name is defined, or [] otherwise. */
value type_resolve(value f)
	{
	value x = arg(type_string,&f->R);
	const char *name = string_data(x);
	return maybe(resolve_name(name));
	}

value top(void)
	{
	return Q(type_resolve);
	}
