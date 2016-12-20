#include <num.h>
#include <value.h>

#include <basic.h>
#include <die.h>
#include <stdio.h> /* perror */
#include <sys/resource.h>
#include <type_limit.h>
#include <type_num.h>

static value op_set_limit(value f, int resource, const char *label)
	{
	if (!f->L) return 0;
	{
	value x = arg(f->R);
	if (x->T == type_num)
		{
		int code;
		struct rlimit rlim;
		rlim_t n = get_double(x);
		rlim.rlim_cur = n;
		rlim.rlim_max = n;
		code = setrlimit(resource,&rlim);
		if (code < 0)
			{
			perror(label);
			die(0);
			}
		f = QI();
		}
	else
		f = reduce_void(f);
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
