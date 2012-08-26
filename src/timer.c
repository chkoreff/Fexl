#include <sys/resource.h>
#include "value.h"
#include "basic.h"
#include "long.h"

/* ru_time next = (next (pair sec usec)) */
void reduce_ru_utime(value f)
	{
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	replace_apply(f, f->R,
		A(A(Q(reduce_pair),Qlong(ru.ru_utime.tv_sec)),
			Qlong(ru.ru_utime.tv_usec)));
	}
