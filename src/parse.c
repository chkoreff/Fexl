/*
Grammar:

exp  =>  empty
exp  =>  term exp
exp  =>  ; exp
exp  =>  \ sym def exp

term =>  sym
term =>  ( exp )

def  =>  empty
def  =>  is term

is   =>  =
is   =>  ==

sym  => name
sym  => string

string => simple_string
string => complex_string

The Fexl parser reads an expression from the input until it reaches -1 (end of
file) or the special token "\\".  The \\ token stops the parser immediately, as
if it had reached end of file.

If you're parsing a possibly malicious script, you should use setrlimit to
impose limits not only on stack depth, but also on total memory usage and CPU
time.

For example, if you parse an infinite stream of '(' characters, you'll see a
segmentation fault due to stack overflow.  That happens due to the default
limits on stack size, without any intervention.  But if you parse an infinite
stream of ' ' characters, it will run forever unless you impose limits on CPU
time with RLIMIT_CPU.  If you parse an infinite stream of 'a' characters, it
will use an unbounded amount of memory which could slow your machine to a crawl
until it finally reaches the very large default limits.  So you should set
RLIMIT_AS, and also RLIMIT_DATA for good measure, to limit the total amount of
memory.
*/

static int ch;                        /* current character */
static FILE *source_fh = 0;           /* current source file */
static const char *source_label = 0;  /* current label of source file */
static long source_line;              /* current line number */
static value source_context;          /* current context */

static void next_ch(void)
	{
	ch = fgetc(source_fh);
	if (ch == '\n') source_line++;
	}

static void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == -1)
			{
			next_ch();
			return;
			}
		else
			next_ch();
		}
	}

static int at_white(void)
	{
	return isspace(ch) || ch == 0;
	}

static void skip_white(void)
	{
	while (1)
		{
		if (!at_white() || ch == -1)
			return;
		else
			next_ch();
		}
	}

static void skip_filler(void)
	{
	while (1)
		{
		if (ch == '#')
			skip_line();
		else if (at_white())
			skip_white();
		else
			return;
		}
	}

static void syntax_error(const char *msg, int line)
	{
	die("%s on line %ld%s%s", msg, line,
		source_label[0] ? " of " : "",
		source_label);
	}

static void undefined_symbol(const char *name, int line)
	{
	warn("Undefined symbol %s on line %ld%s%s", name, line,
		source_label[0] ? " of " : "",
		source_label
		);
	}

static value type_name(value f) { return type_data(f); }
static value type_open(value f) { return type_data(f); }

static int is_null_name(value x)
	{
	return x->T == type_name && x->L != 0 && string_len(x->L) == 0;
	}

static int is_sym(value x)
	{
	return (x->T == type_name || x->T == type_string) && x->L != 0;
	}

static int sym_eq(value x, value y)
	{
	return is_sym(x) && is_sym(y)
		&& x->T == y->T
		&& string_cmp(x->L,y->L) == 0;
	}

static int is_open(value x)
	{
	return x->T == type_open || is_sym(x);
	}

/* Apply f to g, returning an open form if either one is open. */
static value apply(value f, value g)
	{
	value v = A(f,g);
	if (is_open(f) || is_open(g))
		v->T = type_open;
	return v;
	}

static value make_sym(type t, struct buf **buffer, long first_line)
	{
	return V(t,Qstring_buffer(buffer),Qlong(first_line));
	}

/* Collect the content of a string up to the given terminator. */
static value collect_string(const char *term_string, long term_len,
	long first_line)
	{
	struct buf *buffer = 0;
	long match_pos = 0;

	while (1)
		{
		if (ch == term_string[match_pos])
			{
			match_pos++;
			next_ch();
			}
		else if (match_pos > 0)
			{
			if (match_pos >= term_len)
				return make_sym(type_string,&buffer,first_line);
			else
				{
				long i;
				for (i = 0; i < match_pos; i++)
					buf_add(&buffer, term_string[i]);

				match_pos = 0;
				}
			}
		else if (ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			{
			buf_add(&buffer, ch);
			next_ch();
			}
		}
	}

static value parse_simple_string(void)
	{
	long first_line = source_line;
	next_ch();
	return collect_string("\"", 1, first_line);
	}

static value parse_complex_string(void)
	{
	long first_line = source_line;
	struct buf *buffer = 0;

	while (1)
		{
		if (at_white())
			{
			value terminator = Qstring_buffer(&buffer);

			next_ch();
			value sym = collect_string(string_data(terminator),
				string_len(terminator), first_line);

			check(terminator);
			return sym;
			}
		else if (ch == -1)
			syntax_error("Unclosed string terminator", first_line);
		else
			{
			buf_add(&buffer, ch);
			next_ch();
			}
		}
	}

/*
A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest possible rule that
can work.
*/
static value parse_name(int allow_eq)
	{
	long first_line = source_line;
	struct buf *buffer = 0;
	while (1)
		{
		if (
			at_white()
			|| ch == '\\'
			|| ch == '(' || ch == ')'
			|| ch == '[' || ch == ']'
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| (ch == '=' && !allow_eq)
			|| ch == -1
			)
			return make_sym(type_name,&buffer,first_line);
		else
			{
			buf_add(&buffer, ch);
			next_ch();
			}
		}
	}

/* Parse a symbol. */
static value parse_sym(int allow_eq)
	{
	if (ch == '"')
		return parse_simple_string();
	else if (ch == '~')
		return parse_complex_string();
	else
		return parse_name(allow_eq);
	}

static value parse_term(void);
static value parse_exp(void);

static value parse_list(void)
	{
	skip_filler();
	if (ch == ';')
		{
		next_ch();
		return parse_exp();
		}

	value x = parse_term();
	if (is_null_name(x))
		{
		check(x);
		return C;
		}
	else
		return apply(apply(Qitem,x),parse_list());
	}

static value parse_term(void)
	{
	if (ch == '(')
		{
		long first_line = source_line;
		next_ch();
		value exp = parse_exp();
		if (ch != ')')
			syntax_error("Unclosed parenthesis", first_line);

		next_ch();
		return exp;
		}
	else if (ch == '[')
		{
		long first_line = source_line;
		next_ch();
		value exp = parse_list();
		if (ch != ']')
			syntax_error("Unclosed bracket", first_line);

		next_ch();
		return exp;
		}
	else
		return parse_sym(1);
	}

/* Return a shallow copy. */
static value copy(value f)
	{
	return V(f->T,f->L,f->R);
	}

/* Return (S f g), optimizing if possible. */
static value _fuse(value f, value g)
	{
	if (f->L == C)
		{
		if (g == I)
			/* S (C x) I = x */
			/* Copy x because the caller drops f. */
			return copy(f->R);
		else if (g->L == C)
			/* S (C x) (C y) = C (x y) */
			return apply(C,apply(f->R,g->R));
		else
			/* S (C x) y = R x y */
			return apply(apply(R,f->R),g);
		}
	else if (g->L == C)
		/* S x (C y) = L x y */
		return apply(apply(L,f),g->R);
	else
		return apply(apply(S,f),g);
	}

static value fuse(value f, value g)
	{
	value v = _fuse(f,g);
	check(f);
	check(g);
	return v;
	}

/* Abstract the symbol from the body, returning a form which is a function of
that symbol, and no longer contains that symbol. */
static value _abstract(value sym, value body)
	{
	if (body->T == type_open)
		return fuse(_abstract(sym,body->L),_abstract(sym,body->R));
	else if (sym_eq(sym,body))
		return I;
	else
		return apply(C,body);
	}

static value abstract(value sym, value body)
	{
	hold(body);
	value v = _abstract(sym,body);
	drop(body);
	return v;
	}

/*
Parse a lambda form following the initial '\' character.

An ordinary lambda symbol is terminated by white space or '=', for example:

  \x=4
  \y = 5

If you want a lambda symbol to contain '=', you must use white space after the
initial '\'.  This tells the parser that the lambda symbol is terminated by
white space only, and not '='.  For example:

  \ =   = num_eq
  \ ==  = num_eq
  \ !=  = num_ne
  \ <   = num_lt
  \ <=  = num_le
  \ >   = num_gt
  \ >=  = num_ge
*/
static value parse_lambda(long first_line)
	{
	int allow_eq = at_white();
	skip_white();

	/* Parse the symbol (function parameter). */
	value sym = parse_sym(allow_eq);
	if (is_null_name(sym))
		syntax_error("Missing lambda symbol", first_line);

	hold(sym);
	skip_filler();

	first_line = source_line;

	/* Count any '=' signs, up to 2. */
	int count_eq = 0;
	while (ch == '=' && count_eq < 2)
		{
		count_eq++;
		next_ch();
		}

	/* Parse the definition of the symbol if we saw an '=' char. */
	value def = 0;
	if (count_eq)
		{
		skip_filler();
		def = parse_term();

		if (is_null_name(def))
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	value body = abstract(sym,parse_exp());

	/* Produce the result based on the kind of definition used, if any. */
	value result;
	if (count_eq == 0)
		/* no definition */
		result = body;
	else if (count_eq == 1)
		/* = normal definition */
		result = apply(body,def);
	else
		/* == eager definition */
		result = apply(apply(Qquery,def),body);

	drop(sym);
	return result;
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static value parse_factor(void)
	{
	skip_filler();

	if (ch == -1 || ch == ')' || ch == ']')
		return 0;
	else if (ch == '\\')
		{
		long first_line = source_line;
		next_ch();
		if (ch == '\\')
			{
			ch = -1; /* Two backslashes simulates end of file. */
			return 0;
			}
		else
			return parse_lambda(first_line);
		}
	else if (ch == ';')
		{
		next_ch();
		return parse_exp();
		}
	else
		return parse_term();
	}

/* Parse an expression. */
static value parse_exp(void)
	{
	value exp = I;

	while (1)
		{
		value val = parse_factor();
		if (val == 0) return exp;
		exp = exp == I ? val : apply(exp,val);
		}
	}

/* Return the first symbol in the value, if any, in left to right order. */
static value first_sym(value f)
	{
	if (is_sym(f)) return f;
	if (f->T != type_open) return 0;

	value x = first_sym(f->L);
	if (x) return x;
	return first_sym(f->R);
	}

/* Return the definition of the symbol in the current context, returning [def].
Strings are defined literally.  Names are defined using the current context,
which is applied to the name and evaluated.  If a name is undefined, we report
it as an error and use the symbol itself as the definition.  In that case, the
is_open() check on the final expression will be true and the program will exit
with status 1. */

static value resolve_sym(value sym)
	{
	if (sym->T == type_string)
		return item(sym->L,C);

	value result = A(source_context,sym->L);
	result = eval(A(A(result,C),Qitem));
	if (is_item(result))
		return result;

	const char *name = string_data(sym->L);
	long line = long_val(sym->R);
	undefined_symbol(name,line);

	check(result);
	return item(sym,C);
	}

/* Resolve all symbols in the value using the current context, returning a
value with no more symbols and all the definitions baked in. */
static value resolve_all(value f)
	{
	value sym = first_sym(f);

	if (sym == 0) return f;

	value result = resolve_sym(sym);
	value exp = apply(resolve_all(abstract(sym,f)),result->L->R);
	check(result);
	return exp;
	}

/* Parse a fully resolved value. */
static value parse_value(FILE *fh,  const char *label, value context,
	long *p_line)
	{
	source_fh = fh;
	source_label = label;
	source_context = context;
	source_line = *p_line;

	hold(source_context);
	next_ch();
	value exp = parse_exp();

	if (ch != -1)
		syntax_error("Extraneous input", source_line);

	*p_line = source_line;

	exp = resolve_all(exp);
	if (is_open(exp))
		die(0);

	drop(source_context);
	return exp;
	}

value parse_file(const char *name, value context)
	{
	FILE *fh = name[0] ? fopen(name,"r") : stdin;
	if (fh == 0)
		die("Can't open file %s", name);

	long line = 1;
	return parse_value(fh, name, context, &line);
	}

/* (parse_stream fh context label line)
Parse the stream with the given context, label, and initial line number.
Return (\: : exp line), where exp is the parsed expression, and line is the
updated line number.
*/
static value type_parse_stream(value f)
	{
	if (!f->L || !f->L->L || !f->L->L->L || !f->L->L->L->L) return 0;

	value arg_fh = arg(type_file,f->L->L->L->R);
	value arg_context = f->L->L->R;
	value arg_label = arg(type_string,f->L->R);
	value arg_line = arg(type_long,f->R);

	long line = long_val(arg_line);
	value g = parse_value(file_val(arg_fh), string_data(arg_label),
		arg_context, &line);

	if (arg_fh != f->L->L->L->R || arg_label != f->L->R || arg_line != f->R)
		{
		check(arg_fh);
		check(arg_label);
		check(arg_line);
		}

	return yield(yield(I,g),Qlong(line));
	}

/* (use file) Read context from file and parse remainder of source in that
context. */
static value type_use(value f)
	{
	if (!f->L) return 0;

	/*TODO clean up */
	FILE *orig_fh = source_fh;
	const char *orig_label = source_label;
	long orig_line = source_line;

	value arg_file = arg(type_string,f->R);
	const char *local_path = string_data(arg_file);

	value string_append = Q(type_string_append);
	value path;
	path = Q(type_base_path);
	path = A(A(string_append,path),Qstring(local_path));
	path = eval(path);

	const char *full_path = string_data(path);

	FILE *fh = fopen(full_path,"r");
	if (fh == 0)
		{
		fprintf(stderr,"Could not open %s\n", full_path);
		die(0);
		}

	/*TODO always Q(type_resolve) here?  Can we use the "current" context?
	We'll probably make a version that takes current fh, label, line, and
	context as parameters.  That way we're not counting on global variables
	remaining in a particular state.
	*/
	long line = 1;
	value use_context = eval(
		parse_value(fh, local_path, Q(type_resolve), &line));
	check(path);

	value g = parse_value(orig_fh, orig_label, use_context, &orig_line);

	if (arg_file != f->R)
		check(arg_file);

	return g;
	}

value resolve_parse(const char *name)
	{
	if (strcmp(name,"use") == 0) return Q(type_use);
	if (strcmp(name,"parse_stream") == 0) return Q(type_parse_stream);
	if (strcmp(name,"source_fh") == 0) return Qfile(source_fh,0);
	if (strcmp(name,"source_label") == 0) return Qstring(source_label);
	if (strcmp(name,"source_line") == 0) return Qlong(source_line);
	if (strcmp(name,"source_context") == 0) return source_context;
	return 0;
	}
