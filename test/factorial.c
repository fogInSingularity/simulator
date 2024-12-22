#include "minilib.h"

#if !defined (__riscv)
#error "Use risc-v compiler"
#endif // __riscv

unsigned factorial(unsigned n);

void _start() {
    unsigned n = 12;

    unsigned res = factorial(n);

    asm volatile("ebreak" : : : "memory");
}

unsigned factorial(unsigned n) {
    if (n == 1 || n == 0) {
        return 1;
    }

    return n * factorial(n - 1);
}

