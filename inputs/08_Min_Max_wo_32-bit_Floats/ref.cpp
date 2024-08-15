void minmax(float a, float b, float c, float &mn, float &mx) {
    mn = mx = a;
    if (mn > b) mn = b;
    else if (mx < b) mx = b;
    if (mn > c) mn = c;
    else if (mx < c) mx = c;
}