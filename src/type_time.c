#include <stdint.h>
#include <str.h>
#include <value.h>

#include <format.h>
#include <standard.h>
#include <sys/time.h>
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
	return Qnum(n);
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

static string microtime(void)
	{
	uint64_t n;
	struct timeval sys_time;
	gettimeofday(&sys_time,0);
	n = ((uint64_t)sys_time.tv_sec * 1000000) + sys_time.tv_usec;
	return str_new_data0(format_uint64_t(n));
	}

value type_microtime(value f)
	{
	(void)f;
	return Qstr(microtime());
	}
