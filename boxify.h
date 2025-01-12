
extern void boxify_start(int width);

extern void boxify_line(void (*callback)(wchar_t *str), wchar_t *str);

extern void boxify_end(void (*callback)(wchar_t *str));
