/* exit status : exits with the given status */
value type_exit(value f)
	{
	long status = long_val(arg(type_long,&f->R));
	exit(status);
	return 0;
	}

/* ? x y = y x, but with x evaluated first.  Use this to force eager evaluation
when necessary. */
value type_query1(value f)
	{
	return A(f->R,eval(&f->L->L));
	}

value type_query(value f)
	{
	return V(type_query1,f->R,0);
	}

static long get_RLIMIT(const char *name, int *ok)
	{
	*ok = 1;
	if (strcmp(name,"AS") == 0) return RLIMIT_AS;
	if (strcmp(name,"CORE") == 0) return RLIMIT_CORE;
	if (strcmp(name,"CPU") == 0) return RLIMIT_CPU;
	if (strcmp(name,"DATA") == 0) return RLIMIT_DATA;
	if (strcmp(name,"FSIZE") == 0) return RLIMIT_FSIZE;
	if (strcmp(name,"LOCKS") == 0) return RLIMIT_LOCKS;
	if (strcmp(name,"MEMLOCK") == 0) return RLIMIT_MEMLOCK;
	if (strcmp(name,"MSGQUEUE") == 0) return RLIMIT_MSGQUEUE;
	if (strcmp(name,"NICE") == 0) return RLIMIT_NICE;
	if (strcmp(name,"NOFILE") == 0) return RLIMIT_NOFILE;
	if (strcmp(name,"NPROC") == 0) return RLIMIT_NPROC;
	if (strcmp(name,"RSS") == 0) return RLIMIT_RSS;
	if (strcmp(name,"RTPRIO") == 0) return RLIMIT_RTPRIO;
	if (strcmp(name,"RTTIME") == 0) return RLIMIT_RTTIME;
	if (strcmp(name,"SIGPENDING") == 0) return RLIMIT_SIGPENDING;
	if (strcmp(name,"STACK") == 0) return RLIMIT_STACK;
	*ok = 0;
	return -1;
	}

value resolve_RLIMIT(const char *name)
	{
	int ok;
	long limit = get_RLIMIT(name,&ok);
	return ok ? Qlong(limit) : 0;
	}

static void do_setrlimit(long resource, long limit) /*TODO in fexl */
	{
	struct rlimit rlim;
	rlim.rlim_cur = limit;
	rlim.rlim_max = limit;
	int status = setrlimit(resource, &rlim);
	(void)status;
	}

void use_safe_limits(void)
	{
	do_setrlimit(RLIMIT_STACK,400000000);
	do_setrlimit(RLIMIT_DATA,800000000);
	do_setrlimit(RLIMIT_AS,800000000);
	do_setrlimit(RLIMIT_CPU,20);
	}
