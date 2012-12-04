/* A var is a mutable variable where you can put and get values.  This can help
with things like caching, simulating a dynamic entity such as a file system or
human user, redefining print to capture output in a memory buffer, etc. */

value type_var(value f)
	{
	return type_error();
	}

struct atom_var
	{
	void (*free)(struct atom_var *);
	value val;
	};

static void free_var(struct atom_var *x)
	{
	drop(x->val);
	free_memory(x, sizeof(struct atom_var));
	}

/* (var_new val next) Return a new variable with the given value. */
value type_var_new(value f)
	{
	if (!f->L->L) return 0;
	struct atom_var *x = new_memory(sizeof(struct atom_var));
	x->free = free_var;
	x->val = f->L->R;
	hold(x->val);
	return A(f->R,V(type_var,0,(value)x));
	}

static struct atom_var *arg_var(value *pos)
	{
	arg(type_var,pos);
	return (struct atom_var *)(*pos)->R;
	}

/* (var_get var next) Get current value of variable. */
value type_var_get(value f)
	{
	if (!f->L->L) return 0;
	struct atom_var *x = arg_var(&f->L->R);
	return A(f->R,x->val);
	}

/* (var_put var val) Put new value in variable. */
value type_var_put(value f)
	{
	if (!f->L->L) return 0;
	struct atom_var *x = arg_var(&f->L->R);
	hold(f->R);
	drop(x->val);
	x->val = f->R;
	return I;
	}

value resolve_var(const char *name)
	{
	if (strcmp(name,"new") == 0) return Q(type_var_new);
	if (strcmp(name,"get") == 0) return Q(type_var_get);
	if (strcmp(name,"put") == 0) return Q(type_var_put);
	return 0;
	}
