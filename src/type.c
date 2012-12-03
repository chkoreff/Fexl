value type_is_atom(value f)
	{
	value x = eval(&f->R);
	return boolean(x->L == 0 && x->R != 0);
	}

value type_is_list(value f)
	{
	value x = eval(&f->R);
	return boolean(x->T == type_C || x->T == type_item2);
	}

value type_is_type1(value f)
	{
	value x = eval(&f->L->L);
	value y = eval(&f->R);
	return boolean(x->T == y->T);
	}

value type_is_type(value f)
	{
	return V(type_is_type1,f->R,0);
	}

value resolve_is(const char *name)
	{
	if (strcmp(name,"atom") == 0) return Q(type_is_atom);
	if (strcmp(name,"list") == 0) return Q(type_is_list);
	if (strcmp(name,"type") == 0) return Q(type_is_type);
	return 0;
	}
