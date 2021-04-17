/* The define ensures that strptime is accessible. */
#define _GNU_SOURCE
#include <stdint.h>
#include <sys/time.h>
#include <time.h>

#include <str.h>
#include <value.h>

#include <basic.h>
#include <format.h>
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

static const char *time_format = "%Y-%m-%d %H:%M:%S";

/* Format an epoch time as "YYYY-MM-DD HH:MM:SS". */
static value op_strftime(value f, struct tm *(*convert)(const time_t *))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		time_t n = get_double(x);
		struct tm *tm = convert(&n);
		char buf[32];
		strftime(buf,32,time_format,tm);
		f = Qstr0(buf);
		}
	else
		f = hold(&Qvoid);
	drop(x);
	return f;
	}
	}

/* Convert "YYYY-MM-DD HH:MM:SS" to an epoch time. */
static value op_strptime(value f, time_t (*convert)(struct tm *tm))
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_str)
		{
		const char *str = str_data(x);
		struct tm tm;
		char *result = strptime(str,time_format,&tm);
		if (result)
			f = Qnum(convert(&tm));
		else
			f = hold(&Qvoid);
		}
	else
		f = hold(&Qvoid);
	drop(x);
	return f;
	}
	}

/* Convert epoch to string in local time zone. */
value type_localtime(value f)
	{
	return op_strftime(f,localtime);
	}

/* Convert epoch to string in UTC time zone. */
value type_gmtime(value f)
	{
	return op_strftime(f,gmtime);
	}

/* Convert string to epoch in local time zone. */
value type_timelocal(value f)
	{
	return op_strptime(f,timelocal);
	}

/* Convert string to epoch in UTC time zone. */
value type_timegm(value f)
	{
	return op_strptime(f,timegm);
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
