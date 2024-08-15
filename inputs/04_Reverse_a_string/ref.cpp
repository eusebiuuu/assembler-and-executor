void strrev(char *str) {
    int i;
    int sz = strlen(str);
    for (i = 0;i < sz / 2;i++) {
        char c = str[i];
        str[i] = str[sz - i - 1];
        str[sz - i - 1] = c;
    }
}