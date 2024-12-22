#!/bin/bash

~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-as -mabi=ilp32 -march=rv32i $1 -o $1.o
~/code/sims/ricsv-toolchain/toolchain/bin/riscv64-unknown-elf-ld $1.o -o $1.out -melf32lriscv

rm $1.o
