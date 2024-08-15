LL *addll(LL *list, LL *element) {
    element->next = list;
    return element;
}