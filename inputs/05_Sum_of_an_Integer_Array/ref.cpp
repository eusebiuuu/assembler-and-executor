int arraysum(int a[], int size) {
    int ret = 0;
    int i;
    for (i = 0;i < size;i++) {
        ret = ret + a[i];
    }
    return ret;
}