extern int cur_ch;
extern unsigned long cur_line;
extern void skip(void);
extern int at_white(void);
extern void skip_white(void);
extern void buf_keep(buffer buf);
extern void skip_filler(void);
extern int collect_to_white(struct buffer *buf);
extern int collect_to_ch(struct buffer *buf, const char t_ch);
extern int collect_string(struct buffer *buf, const char *end,
	unsigned long len);
extern int collect_tilde_string(struct buffer *buf);
extern value read_stream(value x, value read);