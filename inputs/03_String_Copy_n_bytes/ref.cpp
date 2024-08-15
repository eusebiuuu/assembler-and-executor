// from the strncpy man pages
char *strncpy(char *dst, const char *src, unsigned long n) {
    unsigned long i;
    for (i = 0; i < n && src[i] != '\0'; i++)
        dst[i] = src[i];
    for ( ; i < n; i++)
        dst[i] = '\0';
    return dst;
}