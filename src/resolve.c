/* This is the standard context which supplies definitions for any symbols used
in a Fexl program. */

static value type_resolve(value f);

static value resolve(const char *name)
	{
	value f;
	if ((f = resolve_long(name))) return f;
	if ((f = resolve_double(name))) return f;
	if ((f = resolve_string(name))) return f;
	if ((f = resolve_basic(name))) return f;
	if ((f = resolve_file(name))) return f;
	if ((f = resolve_system(name))) return f;
	if ((f = resolve_var(name))) return f;
	if ((f = resolve_parse(name))) return f;

	/* LATER more functions
	if (strcmp(name,"lib") == 0) return Q(type_lib);
	*/

	if (strcmp(name,"resolve") == 0) return Q(type_resolve);
	return 0;
	}

/* (resolve name) returns [def] if name is defined, or [] otherwise. */
static value type_resolve(value f)
	{
	if (!f->L) return 0;
	value arg_name = arg(type_string,f->R);
	const char *name = string_data(arg_name);
	value def = resolve(name);

	if (arg_name != f->R)
		{
		check(arg_name);
		f = 0;
		}
	return replace_maybe(f, def);
	}

/* This is the standard context for Fexl programs. */
value standard_context(void)
	{
	value string_append = Q(type_string_append);
	value path;
	path = Q(type_base_path);
	path = A(A(string_append,path),Qstring("share/fexl/main.fxl"));
	path = eval(path);

	const char *full_path = string_data(path);
	value context = parse_file(full_path, Q(type_resolve));
	check(path);
	return context;
	}
