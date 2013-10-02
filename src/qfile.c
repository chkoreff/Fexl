#include <stdio.h>
#include "str.h"

#include "value.h"
#include "basic.h"
#include "qfile.h"

FILE *as_file(value f)
	{
	return (FILE *)f->R;
	}

value type_file(value f)
	{
	if (!f->N)
		{
		FILE *fh = as_file(f);
		if (fileno(fh) > 2)  /* don't close stdin, stdout, or stderr */
			fclose(fh);
		}
	return f;
	}

FILE *get_file(value f)
	{
	if (f->T != type_file) bad_type();
	return as_file(f);
	}

/* Make a file value from the given file handle. */
value Qfile(FILE *fh)
	{
	return V(type_file,0,(value)fh);
	}
