#!/bin/bash

~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-gcc -nostartfiles -Og -nostdlib $1 -o $1.asm -mabi=ilp32 -march=rv32i -S
~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-as -mabi=ilp32 -march=rv32i $1.asm -o $1.o
~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-ld $1.o -o $1.out -melf32lriscv

rm $1.asm $1.o

# ./compile_c_nolib.sh factorial.c 
