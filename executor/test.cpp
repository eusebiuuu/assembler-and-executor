#include <iostream>
#include <fstream>
#include <bitset>
using namespace std;

fstream fin("../assembler/memory");

int main() {
    char c;
    fin.read(&c, 1);
    bitset<8> x = c;
    cout << x << '\n';
    fin.seekg(1);
    fin.put(7);
    fin.seekg(0);
    return 0;
}