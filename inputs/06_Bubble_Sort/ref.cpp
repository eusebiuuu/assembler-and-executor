void bubsort(long *list, int size) {
    bool swapped;
    do {
        swapped = false;
        for (int i = 1;i < size;i++) {
            if (list[i-1] > list[i]) {
               swapped = true;
               long tmp = list[i-1];
               list[i-1] = list[i];
               list[i] = tmp;
            }
        }
    } while (swapped);
}