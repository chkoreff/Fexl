#include <value.h>

#include <basic.h>
#include <load.h>

static void clear_lib(value f)
	{
	(void)f;
	}

static value make_load(void predefine(void))
	{
	static struct value clear = {{.N=0}, {.clear=clear_lib}};
	return V(type_void,&clear,(value)predefine);
	}

static value type_load(value f)
	{
	void (*predefine)(void) = f->R->v_ptr;
	predefine();
	return hold(QI);
	}

void define_lib(const char *name, void predefine(void))
	{
	define(name,A(Q(type_load),make_load(predefine)));
	}
