typedef int (*input)(void);
/* Use (void *) instead of (FILE *) to avoid include stdio.h everywhere. */
extern void *cur_in;
extern input getd; /* current input routine */
extern void get_from(void *in);
