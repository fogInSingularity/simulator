    .section .data
msg:    .asciz "hello world\n"       # The string to be printed
len:    .word 12                    # Length of the string, including the newline

    .section .text
    .globl _start

_start:
    # Load the arguments for the write system call
    la a1, msg          # Load the address of the message into a1
    li a2, 12           # Load the length of the message into a2
    li a0, 1            # File descriptor 1 (stdout)
    li a7, 64           # Syscall number for write
    ecall               # Make the system call

    # Use ebreak to stop execution
    ebreak
