#include <run.h>

int main(int argc, const char *argv[])
	{
	const char *name = argc > 1 ? argv[1] : "";
	run(name);
	}
