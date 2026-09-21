#include <str.h>
#include <value.h>

#include <basic.h>
#include <type_record.h>
#include <type_str.h>
#include <type_sym.h>

void define(const char *name, value val)
	{
	value key = Qstr0(name);
	record_set(Qstd,key,val);
	drop(key);
	}
