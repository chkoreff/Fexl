value lib(const char *name)
	{
	value string_append = Q(type_string_append);
	value path;
	path = A(Q(type_base_path),I);
	path = A(A(string_append,path),Qstring("lib/fexl/"));
	path = A(A(string_append,path),Qstring(name));
	path = A(A(string_append,path),Qstring(".so"));

	hold(path);
	eval(&path);

	void *lib = dlopen(string_data(path), RTLD_LAZY);
	if (lib == 0)
		die("Error opening library %s", dlerror());
	drop(path);

	value (*top)(void);
	top = dlsym(lib,"top");
	if (top == 0)
		die("Error getting symbol in %s", dlerror());

	return top();
	}
