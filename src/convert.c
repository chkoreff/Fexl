#include <convert.h>
#include <ctype.h>
#include <stdlib.h> // strtod

// I use the standard strtod(3) to convert a string to a double.  Problem is,
// that routine will succeed on "inf", "-inf", "nan", and "-nan".  If those
// always parsed as numeric values, it would be impossible to use any of those
// names as a lambda symbol.  Besides, the semantics of those values are ugly
// and arcane.
//
// Consequently I do a couple of checks before calling strtod(3).  The first
// byte must be either a digit 0-9, or '-', '.', or '+'.  If the first byte is
// '-', the the second byte must be either a digit 0-9 or '.'.  I'm pretty
// certain that is the minimal constraint I can impose which makes strtod
// succeed in all cases EXCEPT the ones mentioned above.

int str0_double(const char *str, double *val)
	{
	char ch;
	if (str == 0) return 0;

	ch = str[0];
	if (!(isdigit(ch) || ch == '-' || ch == '.' || ch == '+'))
		ch = 0;

	if (ch == '-')
		{
		ch = str[1];
		if (!(isdigit(ch) || ch == '.'))
			ch = 0;
		}

	if (ch)
		{
		char *end;
		*val = strtod(str, &end);
		return (*end == '\0');
		}
	else
		return 0;
	}
