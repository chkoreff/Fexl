#include <unistd.h> /* readlink */
#include <file.h>
#include <memory.h>
#include <str.h>

/* Call readlink, returning a string. */
struct str *safe_readlink(const char *path)
	{
	char *buf;
	long len;
	long size = 256;

	while (1)
		{
		buf = new_memory(size);
		len = readlink(path, buf, size);
		if (len < 0) len = 0; /* Return null string if system error. */

		if (len == size)
			{
			/* Used all available space, so the result might be truncated. */
			free_memory(buf, size);
			size = 2 * size;
			}
		else
			{
			/* Used less than available space, so the result fits fine. */
			struct str *result = str_new_data(buf,len);
			free_memory(buf, size);
			return result;
			}
		}
	}

/*
Get the base path of the currently running program, ending in "/".  If we could
not get the path due to a system error, return "".

First we get the full path of the program, for example "/PATH/bin/fexl".  This
is equivalent to the $0 variable in the /bin/sh program.  Then we strip off the
last two legs of that path, returning "/PATH/".
*/
struct str *base_path(void)
	{
	struct str *path_exe = safe_readlink("/proc/self/exe");

	const char *buf = path_exe->data;
	long len = path_exe->len;

	int i;
	for (i = 0; i < 2; i++)
		while (len > 0 && buf[--len] != '/')
			;

	if (buf[len] == '/') len++;  /* keep the slash */
	struct str *result = str_new_data(buf, len);
	str_free(path_exe);
	return result;
	}
