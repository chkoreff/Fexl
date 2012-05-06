#include <sys/resource.h>
#include "value.h"
#include "basic.h"
#include "long.h"

/* ru_time next = (next (pair sec usec)) */
value fexl_ru_utime(value f)
	{
	if (!f->L) return 0;
	struct rusage ru;
	getrusage(RUSAGE_SELF, &ru);
	return A(f->R,
		A(A(Q(fexl_pair),Qlong(ru.ru_utime.tv_sec)),
			Qlong(ru.ru_utime.tv_usec)));
	}
