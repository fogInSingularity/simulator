#ifndef MINILIB_H_
#define MINILIB_H_

int __mulsi3(int a, int b);
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

#endif // MINILIB_H_
