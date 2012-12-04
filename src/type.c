value type_is_atom(value f)
	{
	value x = eval(&f->R);
	return boolean(x->L == 0 && x->R != 0);
	}

value type_is_list(value f)
	{
	value x = eval(&f->R);
	return boolean(x->T == type_C || (x->T == type_item && x->L && x->L->L));
	}

/* (is_type x y) is true if x and y have the same type. */
value type_is_type(value f)
	{
	if (!f->L->L) return 0;
	value x = eval(&f->L->R);
	value y = eval(&f->R);
	return boolean(x->T == y->T);
	}

value resolve_is(const char *name)
	{
	if (strcmp(name,"atom") == 0) return Q(type_is_atom);
	if (strcmp(name,"list") == 0) return Q(type_is_list);
	if (strcmp(name,"type") == 0) return Q(type_is_type);
	return 0;
	}
