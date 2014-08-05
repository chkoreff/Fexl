extern unsigned long cur_blocks;
extern unsigned long cur_bytes;
extern unsigned long max_words;
extern unsigned long cur_words;
extern void (*emergency)(void);
extern void *new_memory(unsigned long num_bytes, unsigned long num_words);
extern void free_memory(void *data,
	unsigned long num_bytes, unsigned long num_words);
extern void end_memory(void);
