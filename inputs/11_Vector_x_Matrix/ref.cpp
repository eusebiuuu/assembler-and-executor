void matmul(float dst[3], float matrix[3][3], float vector[3]) {
    int r;
    int c;
    float d;
    for (r = 0;r < 3;r++) {
        d = 0;
        for (c = 0;c < 3;c++) {
             d = d + matrix[r][c] * vector[c];
        }
        dst[r] = d;
    }
}