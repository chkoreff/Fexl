#include <value.h>
#include <basic.h>
#include <buf.h>
#include <ctype.h>
#include <die.h>
#include <parse.h>
#include <str.h>
#include <type_sym.h>

/*
Grammar:
[
exp    => terms
exp    => \ sym def exp

terms  => empty
terms  => term terms
terms  => ; exp

term   => ( exp )
term   => [ terms ]
term   => { exp }
term   => sym

def    => empty
def    => = term
def    => == term

sym    => name
sym    => string

string => simple_string
string => complex_string
]

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

/* The get function reads the next character.  It can be set to read from any
source such as a file or a string. */
static int (*get)(void);

static int ch;            /* current character */
static int line;          /* current line number */
static const char *label; /* current name of source */

static void skip(void)
	{
	ch = get();
	if (ch == '\n') line++;
	}

static void skip_line(void)
	{
	while (1)
		{
		if (ch == '\n' || ch == -1)
			{
			skip();
			return;
			}
		else
			skip();
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
			skip();
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
		label[0] ? " of " : "",
		label);
	}

/*
A name may contain just about anything, except for white space (including NUL)
and a few other special characters.  This is the simplest possible rule that
can work.
*/
static value parse_name(int allow_eq)
	{
	int first_line = line;
	struct buf buf;
	buf_start(&buf);

	while (1)
		{
		if (at_white()
			|| ch == '\\'
			|| ch == '(' || ch == ')'
			|| ch == '[' || ch == ']'
			|| ch == '{' || ch == '}'
			|| ch == ';'
			|| ch == '"'
			|| ch == '~'
			|| ch == '#'
			|| (ch == '=' && !allow_eq)
			|| ch == -1)
			break;
		buf_add(&buf,ch);
		skip();
		}

	struct str *name = buf_finish(&buf);
	return name ? Qsym(name,first_line) : 0;
	}

static struct str *collect_string(const char *term_data, int term_len,
	int first_line)
	{
	struct buf buf;
	buf_start(&buf);
	int match_pos = 0;

	while (1)
		{
		if (match_pos >= term_len)
			break;

		if (ch == term_data[match_pos])
			{
			match_pos++;
			skip();
			}
		else if (match_pos > 0)
			{
			/* Buffer the ones matched so far and start over. */
			int i;
			for (i = 0; i < match_pos; i++)
				buf_add(&buf, term_data[i]);

			match_pos = 0;
			}
		else if (ch == -1)
			syntax_error("Unclosed string", first_line);
		else
			{
			buf_add(&buf,ch);
			skip();
			}
		}

	struct str *str = buf_finish(&buf);
	if (str == 0) str = str_new_data("",0);
	return str;
	}

static value parse_simple_string(void)
	{
	int first_line = line;
	skip();
	struct str *str = collect_string("\"", 1, first_line);
	return Qsym(str,-first_line);
	}

static value parse_complex_string(void)
	{
	int first_line = line;

	struct buf buf;
	buf_start(&buf);

	while (1)
		{
		skip();
		if (ch == -1 || isspace(ch))
			break;
		buf_add(&buf,ch);
		}

	struct str *term = buf_finish(&buf);
	if (ch == -1 || term == 0)
		syntax_error("Incomplete string terminator", first_line);

	skip();
	struct str *str = collect_string(term->data, term->len, first_line);
	str_free(term);
	return Qsym(str,-first_line);
	}

static value parse_symbol(int allow_eq)
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
		skip();
		return parse_exp();
		}

	value x = parse_term();
	return x == 0 ? C : app(app(Qcons,x),parse_list());
	}

static value parse_term(void)
	{
	int first_line = line;
	value exp;

	if (ch == '(') /* parenthesized expression */
		{
		skip();
		exp = parse_exp();
		if (ch != ')')
			syntax_error("Unclosed parenthesis", first_line);
		skip();
		}
	else if (ch == '[') /* list */
		{
		skip();
		exp = parse_list();
		if (ch != ']')
			syntax_error("Unclosed bracket", first_line);
		skip();
		}
	else if (ch == '{') /* symbolic form */
		{
		skip();
		exp = A(I,parse_exp());
		if (ch != '}')
			syntax_error("Unclosed brace", first_line);
		skip();
		}
	else
		exp = parse_symbol(1);

	return exp;
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
	value sym = parse_symbol(allow_eq);
	if (sym == 0)
		syntax_error("Missing lambda symbol", first_line);

	skip_filler();

	first_line = line;

	/* Count any '=' signs, up to 2. */
	int count_eq = 0;
	while (ch == '=' && count_eq < 2)
		{
		count_eq++;
		skip();
		}

	/* Parse the definition of the symbol if we saw an '=' char. */
	value def = 0;
	if (count_eq)
		{
		skip_filler();
		def = parse_term();
		if (def == 0)
			syntax_error("Missing definition", first_line);
		}

	/* Parse the body of the function. */
	value body = lam(sym,parse_exp());

	/* Produce the result based on the kind of definition used, if any. */
	value result;
	if (count_eq == 0)
		/* no definition */
		result = body;
	else if (count_eq == 1)
		{
		/* = normal definition */
		if (body == I)
			result = def;
		else
			result = app(body,def);
		}
	else
		/* == eager definition */
		result = app(app(query,def),body);

	return result;
	}

/* Parse the next factor of an expression.  Return 0 if no factor found. */
static value parse_factor(void)
	{
	skip_filler();
	if (ch == -1)
		return 0;
	else if (ch == '\\')
		{
		int first_line = line;
		skip();
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
		skip();
		return parse_exp();
		}
	else
		return parse_term();
	}

static value parse_exp(void)
	{
	value exp = 0;
	while (1)
		{
		value val = parse_factor();
		if (val == 0) break;
		exp = (exp == 0) ? val : app(exp,val);
		}
	if (exp == 0) exp = I;
	return exp;
	}

value parse(int _get(void), int *_line, const char *_label)
	{
	/* Save and restore so it's re-entrant. */
	int (*save_get)(void) = get;
	int save_ch = ch;
	int save_line = line;
	const char *save_label = label;

	get = _get;
	ch = 0;
	line = *_line;
	label = _label;

	value exp = parse_exp();
	if (ch != -1)
		syntax_error("Extraneous input", line);

	*_line = line;

	get = save_get;
	label = save_label;
	line = save_line;
	ch = save_ch;

	return exp;
	}
