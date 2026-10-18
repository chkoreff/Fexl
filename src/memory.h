extern unsigned long cur_blocks;
extern unsigned long cur_bytes;
extern void *new_memory(unsigned long num_bytes);
extern void *resize_memory(void *data,
	unsigned long o_size,
	unsigned long n_size);
extern void free_memory(void *data, unsigned long num_bytes);
extern void end_memory(void);
