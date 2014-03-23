int show_atom_base(value f)
	{
	if (f->T == type_C)
		printf("C");
	else if (f->T == type_I)
		printf("I");
	else if (f->T == type_S)
		printf("S");
	else if (f->T == type_R)
		printf("R");
	else if (f->T == type_L)
		printf("L");
	else if (f->T == type_Y)
		printf("Y");
	else if (f->T == type_concat)
		printf("concat");
	else if (f->T == type_str)
		printf("\"%s\"",atom_str(f)->data);
	else if (f->T == type_sym)
		{
		struct sym *sym = atom_sym(f);
		if (sym->line >= 0)
			printf("{%s}",sym->name->data);
		else
			printf("{\"%s\"}",sym->name->data);
		}
	else
		return 0;

	return 1;
	}

void show_value(value f, int show_atom(value))
	{
	if (f->L)
		{
		printf(f->T == type_sym ? "{" : "(");
		show_value(f->L, show_atom);
		printf(" ");
		show_value(f->R, show_atom);
		printf(f->T == type_sym ? "}" : ")");
		}
	else if (show_atom(f))
		;
	else
		printf("_");
	}

void nl(void)
	{
	putchar('\n');
	}
