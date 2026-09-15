#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_record.h>
#include <type_str.h>
#include <type_sym.h>

void define(const char *s_key, value val)
	{
	value key = Qstr0(s_key);
	record_set(Qstd,key,val);
	drop(key);
	}
