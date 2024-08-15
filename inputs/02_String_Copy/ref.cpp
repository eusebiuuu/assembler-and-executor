void stringcopy(char *dst, const char *src) {
    do {
        // Copy the source's byte into the destination's byte
        *dst = *src;
        // We check '\0' here so that we copy the source's \0
        // into the destination.
        if (*src == '\0') break;
        // Advance both the destination and source to the next byte.
        dst++;
        src++;
    } while (true);
}