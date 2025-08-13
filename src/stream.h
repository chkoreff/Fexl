typedef int (*input)(void *source);
extern int cur_ch;
extern unsigned long cur_line;
extern input cur_get;
extern void *cur_source;
extern void skip(void);
extern int at_white(void);
extern void skip_white(void);
extern void buf_keep(buffer buf);
extern void skip_filler(void);
extern int collect_to_white(buffer buf);
extern int collect_to_ch(buffer buf, const char t_ch);
extern int collect_string(buffer buf, const char *end, unsigned long len);
extern int collect_tilde_string(buffer buf);

// Holds the current state of a stream.
struct state
	{
	int cur_ch;
	unsigned long cur_line;
	input cur_get;
	void *cur_source;
	};

extern struct state beg_stream(input get, void *source);
extern void end_stream(struct state s);
