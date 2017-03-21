#if DEV
extern unsigned long max_bytes;
#endif
extern void *new_memory(unsigned long num_bytes);
extern void free_memory(void *data, unsigned long num_bytes);
extern void end_memory(void);
