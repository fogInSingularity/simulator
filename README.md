# Functional risc-v rv32i cpu simulator

## About:

This simulator is written as a homework for the functional simulator course from the MIPT-based Microprocessor Technology Department.
At the moment it supports isa rv32i except for fence instructions. There is also support for write and read syscall.
Files for execution must be in ELF format.

## Installation:

### Prerequisites:
1. A C++ compiler (g++ recommended)
2. CMake (version 3.15 or later)
3. git

Clone the repository:
```bash
git clone https://github.com/fogInSingularity/simulator
```
Navigate to the project directory:
```bash
cd simulator
```
Configure and build project:
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel `nproc`
```

## Usage:

To run the simulator, use the following command:
```bash
./build/simulator <target_execuable>
```
