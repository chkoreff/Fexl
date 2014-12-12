#include <input.h>
#include <str.h>
#include <str_input.h>

static string source;
static unsigned long pos;

static int get(void)
	{
	return pos < source->len ? source->data[pos++] : -1;
	}

void get_from_string(string x, struct str_input *save)
	{
	save->getd = getd;
	save->source = source;
	save->pos = pos;

	source = x;
	pos = 0;
	getd = get;
	}

void restore_str_input(struct str_input *save)
	{
	getd = save->getd;
	source = save->source;
	pos = save->pos;
	}
