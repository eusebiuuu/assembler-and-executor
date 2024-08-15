int main() {
    int a, b, c;
    printf("Enter a, b, and c: ");
    scanf("%d %d %d", &a, &b, &c);
    int tmp = cfunc(a, b, c);       //external c function
    printf("Result = %d\n", tmp);
}