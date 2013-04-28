/* A var is a mutable variable where you can put and get values.  This can help
with things like caching, simulating a dynamic entity such as a file system or
human user, redefining print to capture output in a memory buffer, etc. */

value type_var(value f) { return type_C(f); } /*TODO*/

static void free_var(value f)
	{
	drop(f->L);
	}

/* (var_new val) Return a new variable with the given value. */
static value type_var_new(value f)
	{
	if (!f->L) return 0;
	value x = V((type)free_var,f->R,0);
	return V(type_var,0,x);
	}

/* (var_get var) Get current value of variable. */
static value type_var_get(value f)
	{
	if (!f->L) return 0;
	value var = arg(type_var,f->R);
	value val = var->R->L;

	if (var != f->R)
		check(var);
	return val;
	}

/* (var_put var val) Put new value in variable. */
static value type_var_put(value f)
	{
	if (!f->L || !f->L->L) return 0;
	value var = arg(type_var,f->L->R);
	value new = f->R;

	hold(new);
	drop(var->R->L);
	var->R->L = new;

	if (var != f->L->R)
		check(var);
	return I;
	}

static value resolve_var_prefix(const char *name)
	{
	if (strcmp(name,"new") == 0) return Q(type_var_new);
	if (strcmp(name,"get") == 0) return Q(type_var_get);
	if (strcmp(name,"put") == 0) return Q(type_var_put);
	return 0;
	}

value resolve_var(const char *name)
	{
	if (strncmp(name,"var_",4) == 0) return resolve_var_prefix(name+4);
	return 0;
	}