# Функциональный симулятор risc-v rv32i процессора

## Установка:

```bash
git clone https://github.com/fogInSingularity/simulator
cd simulator
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel `nproc`
```

## Использование:

```bash
./build/simulator <target_execuable>
```
