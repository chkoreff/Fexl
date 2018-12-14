#include <num.h>
#include <str.h>
#include <value.h>

#include <standard.h>
#include <time.h>
#include <type_num.h>
#include <type_str.h>
#include <type_time.h>

/* Return the current time as the number of seconds since the Epoch,
1970-01-01 00:00:00 +0000 (UTC). */
value type_time(value f)
	{
	time_t n;
	(void)f;
	time(&n);
	return Qnum0(n);
	}

/* Format an epoch time as "YYYY-MM-DD HH:MM:SS". */
value type_localtime(value f)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		time_t n = get_double(x);
		struct tm *tm = localtime(&n);
		char buf[32];
		strftime(buf,32,"%Y-%m-%d %H:%M:%S",tm);
		f = Qstr0(buf);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	}
	}
