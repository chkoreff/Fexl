#include "value.h"
#include "string.h"
#include "var.h"

struct type type_var = { reduce_none, hold_string, drop_string, "var" };
