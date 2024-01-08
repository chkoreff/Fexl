// The define ensures that strptime is accessible.
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

// Return the current time as the number of seconds since the Epoch,
// 1970-01-01 00:00:00 +0000 (UTC).
value type_time(value fun, value f)
	{
	time_t n;
	time(&n);
	return Qnum(n);
	(void)fun;
	(void)f;
	}

static const char *time_format = "%Y-%m-%d %H:%M:%S";

// Format an epoch time as "YYYY-MM-DD HH:MM:SS".
static value op_strftime(value fun, value f,
	struct tm *(*convert)(const time_t *))
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		time_t n = x->v_double;
		struct tm *tm = convert(&n);
		char buf[32];
		strftime(buf,sizeof(buf),time_format,tm);
		f = Qstr0(buf);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

// Convert "YYYY-MM-DD HH:MM:SS" to an epoch time.
static value op_strptime(value fun, value f,
	time_t (*convert)(struct tm *tm))
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
			f = hold(Qvoid);
		}
	else
		f = hold(Qvoid);
	drop(x);
	return f;
	(void)fun;
	}

// Convert epoch to string in local time zone.
value type_localtime(value fun, value f)
	{
	return op_strftime(fun,f,localtime);
	}

// Convert epoch to string in UTC time zone.
value type_gmtime(value fun, value f)
	{
	return op_strftime(fun,f,gmtime);
	}

// Convert string to epoch in local time zone.
value type_timelocal(value fun, value f)
	{
	return op_strptime(fun,f,timelocal);
	}

// Convert string to epoch in UTC time zone.
value type_timegm(value fun, value f)
	{
	return op_strptime(fun,f,timegm);
	}

static string microtime(void)
	{
	uint64_t n;
	struct timeval sys_time;
	gettimeofday(&sys_time,0);
	n = ((uint64_t)sys_time.tv_sec * 1000000) + sys_time.tv_usec;
	return str_new_data0(format_uint64_t(n));
	}

value type_microtime(value fun, value f)
	{
	return Qstr(microtime());
	(void)fun;
	(void)f;
	}

// 1:Mon 2:Tue 3:Wed 4:Thu 5:Fri 6:Sat 0:Sun
static int get_dow(int y, int m, int d)
	{
	static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
	if (m < 3)
		y--;
	return (y + y/4 - y/100 + y/400 + t[m-1] + d) % 7;
	}

// Return day of week for y,m,d.
// 1:Mon 2:Tue 3:Wed 4:Thu 5:Fri 6:Sat 7:Sun
value type_dow(value fun, value f)
	{
	if (fun->L == 0) return keep(fun,f);
	if (fun->L->L == 0) return keep(fun,f);
	{
	value a1 = arg(fun->L->R);
	value a2 = arg(fun->R);
	value a3 = arg(f->R);
	if (a1->T == type_num && a2->T == type_num && a3->T == type_num)
		{
		int y = a1->v_double;
		int m = a2->v_double;
		int d = a3->v_double;
		int dow = get_dow(y,m,d);
		if (dow == 0) dow = 7;
		f = Qnum(dow);
		}
	else
		f = hold(Qvoid);
	drop(a1);
	drop(a2);
	drop(a3);
	return f;
	}
	}
