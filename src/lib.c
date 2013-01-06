/*TODO use this to grab a fexl function from a shared library */
value lib(const char *name)
	{
	value string_append = Q(type_string_append);
	value path;
	path = Q(type_base_path);
	path = A(A(string_append,path),Qstring("lib/fexl/"));
	path = A(A(string_append,path),Qstring(name));
	path = A(A(string_append,path),Qstring(".so"));
	path = eval(path);

	void *lib = dlopen(string_data(path), RTLD_LAZY);
	check(path);

	if (lib == 0)
		die("Error opening library %s", dlerror());

	value (*top)(void);
	top = dlsym(lib,"top");
	if (top == 0)
		die("Error getting symbol in %s", dlerror());

	return top();
	}
