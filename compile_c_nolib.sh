#!/bin/bash

# $1 sources
# $2 asm
# $3 obj(?)
# $4 exec

~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-gcc -nostartfiles -Og -nostdlib $1 -o $2 -mabi=ilp32 -march=rv32i -S
~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-as -mabi=ilp32 -march=rv32i $2 -o $3
~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-ld $3 -o $4 -melf32lriscv

# ./compile_c_nolib.sh test/factorial.c test/factorial_c.asm test/factorial.o test/factorial 
