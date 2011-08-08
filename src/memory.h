extern long total_blocks;
extern long total_bytes;
extern long max_bytes;

extern void *new_memory(long num_bytes);
extern void free_memory(void *data, long num_bytes);
extern void finish_memory(void);
