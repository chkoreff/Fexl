value type_file(value f) { return type_data(f); }

FILE *file_val(value f)
	{
	return (FILE *)f->R->L;
	}

static value free_file(value x)
	{
	if (x->L && x->R)
		fclose((FILE *)x->L);
	return 0;
	}

/* Make a file value from the given file handle.  The close flag determines
whether the file handle should be closed automatically when the file object is
no longer used. */
value Qfile(FILE *fh, long close)
	{
	value x = Q(0);
	x->T = free_file;
	x->L = (value)fh;
	x->R = (value)close;
	return V(type_file,0,(value)x);
	}

/* (putchar ch) Write character to stdout. */
static value type_putchar(value f)
	{
	if (!f->L) return 0;
	value arg_ch = arg(type_long,f->R);
	(void)putchar(long_val(arg_ch));
	check(arg_ch);
	return I;
	}

/* getchar : Return next char from stdin. */
static value type_getchar(value f)
	{
	return Qlong(getchar());
	}

/* (fputc fh ch) Write character to the file. */
static value type_fputc(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value arg_fh = arg(type_file,f->L->R);
	value arg_ch = arg(type_long,f->R);

	(void)fputc(long_val(arg_ch),file_val(arg_fh));

	check(arg_fh);
	check(arg_ch);
	return I;
	}

/* (fgetc fh) returns the next character from the file handle. */
static value type_fgetc(value f)
	{
	if (!f->L) return 0;
	value arg_fh = arg(type_file,f->R);
	int ch = fgetc(file_val(arg_fh));
	check(arg_fh);
	return Qlong(ch);
	}

/* (fwrite fh str) Write string to file handle. */
static value type_fwrite(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value arg_fh = arg(type_file,f->L->R);
	value arg_str = arg(type_string,f->R);

	size_t count = fwrite(string_data(arg_str), 1, string_len(arg_str),
		file_val(arg_fh));
	(void)count;  /* Ignore the return count. */

	check(arg_fh);
	check(arg_str);
	return I;
	}

/* (fopen path mode) Open a file and return [fh] or []. */
static value type_fopen(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value arg_path = arg(type_string,f->L->R);
	value arg_mode = arg(type_string,f->R);

	const char *path = string_data(arg_path);
	const char *mode = string_data(arg_mode);

	FILE *fh = fopen(path,mode);
	value g = fh ? Qfile(fh,1) : 0;

	check(arg_path);
	check(arg_mode);
	return replace_maybe(0,g);
	}

/* Call readlink, returning a Fexl string. */
static value safe_readlink(const char *path)
	{
	char *buf;
	long len;
	long size = 256;

	while (1)
		{
		buf = new_memory(size);
		len = readlink(path, buf, size);

		if (len == size)
			{
			/* Used all available space, so the result might be truncated. */
			free_memory(buf, size);
			size = 2 * size;
			}
		else
			{
			/* Used less than available space, so the result fits just fine.
			A system error yields len == -1, but that works robustly. */
			value result = Qstring_vector(buf,len);
			free_memory(buf, size);
			return result;
			}
		}
	}

/* (readlink path) Call readlink(2) on the path. */
static value type_readlink(value f)
	{
	if (!f->L) return 0;
	value arg_path = arg(type_string,f->R);
	value g = safe_readlink(string_data(arg_path));
	check(arg_path);
	return g;
	}

/*
Get the base path of the currently running program, ending in "/".  If we could
not get the path due to a system error, return "".

First we get the full path of the program, for example "/PATH/bin/fexl".  This
is equivalent to the $0 variable in the /bin/sh program.  Then we strip off the
last two legs of that path, returning "/PATH/".
*/
static value get_base_path(void)
	{
	value path = safe_readlink("/proc/self/exe");

	const char *buf = string_data(path);
	long len = string_len(path);

	int i;
	for (i = 0; i < 2; i++)
		while (len > 0 && buf[--len] != '/')
			;

	if (buf[len] == '/') len++;  /* keep the slash */
	value g = Qstring_vector(buf, len);
	check(path);
	return g;
	}

/* (base_path) Return the base path of the current executable.  This is
equivalent to the $0 variable in /bin/sh. */
value type_base_path(value f)
	{
	return get_base_path();
	}

static FILE *get_std_file(const char *name)
	{
	if (strcmp(name,"in") == 0) return stdin;
	if (strcmp(name,"out") == 0) return stdout;
	if (strcmp(name,"err") == 0) return stderr;
	return 0;
	}

static value resolve_std_file(const char *name)
	{
	FILE *fh = get_std_file(name);
	if (fh) return Qfile(fh,0);
	return 0;
	}

value resolve_file(const char *name)
	{
	if (strncmp(name,"std",3) == 0) return resolve_std_file(name+3);
	if (strcmp(name,"putchar") == 0) return Q(type_putchar);
	if (strcmp(name,"getchar") == 0) return Q(type_getchar);
	if (strcmp(name,"fgetc") == 0) return Q(type_fgetc);
	if (strcmp(name,"fputc") == 0) return Q(type_fputc);
	if (strcmp(name,"fwrite") == 0) return Q(type_fwrite);
	if (strcmp(name,"fopen") == 0) return Q(type_fopen);
	if (strcmp(name,"readlink") == 0) return Q(type_readlink);
	if (strcmp(name,"base_path") == 0) return Q(type_base_path);
	return 0;
	}

/*
LATER fclose : (not necessary due to auto-close, but gives you some control.
Just change the close flag to 0
*/

/*LATER fdopen */
/*LATER fflush */
/*LATER file_string */
/*LATER fmemopen */

#if 0

/* sample code here */
{
char *s = "abcdefg";
FILE *fp = fmemopen(s, strlen(s), "r");
int ch;
while (1)
	{
	ch = fgetc(fp);
	if (ch == -1) break;
	printf("ch='%c'\n", ch);
	}
fclose(fp);
}
#endif
