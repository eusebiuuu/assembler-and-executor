// extern "C" is required so we can link the name into assembly
// without knowing how C++ mangles it.
extern "C" {
    int cfunc(int a, int b, int c);
}
// Simple function we're going to call from assembly.
int cfunc(int a, int b, int c) {
    return a + b * c;
}