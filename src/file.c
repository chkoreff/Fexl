value type_file(value f)
	{
	return type_error();
	}

struct atom_file
	{
	void (*free)(struct atom_file *);
	FILE *fh;
	};

static void free_file(struct atom_file *x)
	{
	free_memory(x, sizeof(struct atom_file));
	}

static void close_free_file(struct atom_file *x)
	{
	if (x->fh) fclose(x->fh);
	free_file(x);
	}

/* Make a file value from the given file handle.  The close flag determines
whether the file handle should be closed automatically when the file object is
no longer used. */
value Qfile(FILE *fh, int close)
	{
	struct atom_file *x = new_memory(sizeof(struct atom_file));
	x->free = close ? close_free_file : free_file;
	x->fh = fh;
	return V(type_file,0,(value)x);
	}

FILE *file_val(value f)
	{
	struct atom_file *x = (struct atom_file *)f->R;
	return x->fh;
	}

static FILE *get_std_file(const char *name)
	{
	if (strcmp(name,"in") == 0) return stdin;
	if (strcmp(name,"out") == 0) return stdout;
	if (strcmp(name,"err") == 0) return stderr;
	return 0;
	}

value resolve_std_file(const char *name)
	{
	FILE *fh = get_std_file(name);
	if (fh) return Qfile(fh,0);
	return 0;
	}

/* (putchar ch) Write character to stdout. */
value type_putchar(value f)
	{
	(void)putchar(long_val(arg(type_long,&f->R)));
	return I;
	}

/* (fwrite file str) Write string to file. */
value type_fwrite(value f)
	{
	if (!f->L->L) return 0;
	FILE *fh = file_val(arg(type_file,&f->L->R));
	value str = arg(type_string,&f->R);
	size_t count = fwrite(string_data(str), 1, string_len(str), fh);
	(void)count;  /* Ignore the return count. */
	return I;
	}

/* (fopen path mode) Open a file and return [fh] or []. */
value type_fopen(value f)
	{
	if (!f->L->L) return 0;
	const char *path = string_data(arg(type_string,&f->L->R));
	const char *mode = string_data(arg(type_string,&f->R));

	FILE *fh = fopen(path,mode);
	return maybe(fh ? Qfile(fh,1) : 0);
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
			value result = Qstring_data(buf,len);
			free_memory(buf, size);
			return result;
			}
		}
	}

/* (readlink path next) Call readlink(2) on the path. */
value type_readlink(value f)
	{
	if (!f->L->L) return 0;
	const char *path = string_data(arg(type_string,&f->L->R));
	return A(f->R,safe_readlink(path));
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
	value full_path = safe_readlink("/proc/self/exe");
	hold(full_path);

	const char *buf = string_data(full_path);
	long len = string_len(full_path);

	int i;
	for (i = 0; i < 2; i++)
		while (len > 0 && buf[--len] != '/')
			;

	if (buf[len] == '/') len++;  /* keep the slash */
	value base_path = Qstring_data(buf,len);
	drop(full_path);
	return base_path;
	}

/* (base_path next) Return the base path of the current executable.  This is
equivalent to the $0 variable in /bin/sh. */
value type_base_path(value f)
	{
	return A(f->R,get_base_path());
	}

/*
LATER fclose : (not necessary due to auto-close, but gives you some control.
Just change the type to free_file.

fmemopen str len mode
Some variation thereof, since we don't typically do mutable strings.
But we can use it for parsing from a string.
*/

/*LATER fdopen */
/*LATER fgetc */
/*LATER fputc */
/*LATER fflush */
/*LATER file_string */

#if 0
/*LATER sample code here */
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
