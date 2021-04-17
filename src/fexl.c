#include <value.h>

#include <type_standard.h>

/* Evaluate the named file.  Use stdin if the name is missing or empty.  If the
name designates a directory it behaves like an empty file. */
int main(int argc, const char *argv[])
	{
	eval_standard(argc,argv);
	return 0;
	}
