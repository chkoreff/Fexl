#include <value.h>
#include <basic.h>
#include <die.h>
#include <num.h>
#include <stdio.h> /* perror */
#include <sys/resource.h>
#include <type_num.h>
#include <type_limit.h>

static void set_limit(int resource, const char *label, number n)
	{
	struct rlimit rlim;
	rlim.rlim_cur = *n;
	rlim.rlim_max = *n;
	{
	int code = setrlimit(resource,&rlim);
	if (code < 0)
		{
		perror(label);
		die(0);
		}
	}
	}

static value op_set_limit(value f, int resource, const char *label)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		set_limit(resource,label,data(x));
		f = Q(type_I);
		}
	else
		{
		reduce_void(f);
		f = 0;
		}
	drop(x);
	return f;
	}
	}

/* (limit_time N) Set the CPU time limit in seconds. */
value type_limit_time(value f)
	{
	return op_set_limit(f,RLIMIT_CPU,"limit_time");
	}

/* (limit_memory N) Set the maximum size of the process' virtual memory
(address space) in bytes. */
value type_limit_memory(value f)
	{
	return op_set_limit(f,RLIMIT_AS,"limit_memory");
	}

/* (limit_stack N) Set the maximum size of the process stack in bytes. */
value type_limit_stack(value f)
	{
	return op_set_limit(f,RLIMIT_STACK,"limit_stack");
	}

/* LATER RLIMIT_ FSIZE LOCKS NOFILE NPROC */
