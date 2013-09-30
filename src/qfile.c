#include <stdio.h>
#include "file.h"
#include "str.h"

#include "value.h"
#include "qfile.h"
#include "qstr.h"

FILE *atom_file(value f)
	{
	return (FILE *)f->R;
	}

value type_file(value f)
	{
	if (!f->N) fclose(atom_file(f));
	return f;
	}

/* Make a file value from the given file handle. */
value Qfile(FILE *fh)
	{
	return V(type_file,0,(value)fh);
	}

value type_readlink(value f)
	{
	if (!f->L) return f;
	value x = eval(f->R);
	value y = Qstr(safe_readlink(get_str(x)->data));
	drop(x);
	return y;
	}

value type_base_path(value f)
	{
	return Qstr(base_path());
	}

/*TODO more functions */
