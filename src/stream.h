extern int cur_ch;
extern unsigned long cur_line;
extern void skip(void);
extern int at_white(void);
extern void skip_white(void);
extern void buf_keep(buffer buf);
extern value read_stream(value x, value read);
