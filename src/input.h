typedef int (*input)(void *source);
extern unsigned char char_width(unsigned char ch);
extern string get_utf8(input get, void *source);
