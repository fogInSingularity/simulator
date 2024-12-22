#include "minilib.h"

#if !defined (__riscv)
#error "Use risc-v compiler"
#endif // __riscv

int sum(int a, int b);

void _start() {
    int a = 1;
    int b = 2;

    int res = sum(a, b);

    asm volatile("ebreak" : : : "memory");
}

int sum(int a, int b) {
    return a + b;
}
