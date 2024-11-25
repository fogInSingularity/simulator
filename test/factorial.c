// int main(void) { return 0; }

unsigned factorial(unsigned n);
int __mulsi3(int a, int b);

void _start() {
    unsigned n = 5;

    unsigned res = factorial(n);
}

unsigned factorial(unsigned n) {
    if (n == 1 || n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

int __mulsi3(int a, int b) {
    int result = 0;
    int negative = 0;

    // Handle signs
    if (a < 0) { a = -a; negative = !negative; }
    if (b < 0) { b = -b; negative = !negative; }

    // Perform multiplication (bit-by-bit addition)
    while (b) {
        if (b & 1) {
            result += a;
        }
        b >>= 1;
        a <<= 1;
    }

    return negative ? -result : result;
}

// void print(unsigned n) {
//     unsigned res = factorial(n);
// }
