#include <input.h>
#include <str.h>
#include <str_input.h>

static string source;
static unsigned long pos;

static int get(void)
	{
	return pos < source->len ? source->data[pos++] : -1;
	}

void get_from_string(string x)
	{
	source = x;
	pos = 0;
	getd = get;
	}
