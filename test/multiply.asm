_start:
    addi  x10, x0, 123        /* pc: 0 */
    addi  x11, x0, 150        /* pc: 4 */
    jal   x3, muliply       /* pc: 8 */
    ebreak                  /* pc: 12 */

/* factorial: */
    

/*
input:      a = x10
            b = x11
output:     a * b -> x12
invalidate: x13, x14

x3 = ret address
*/
muliply:
    addi  x12, x0, 0        /* pc: 16 */
    addi  x13, x0, 0        /* pc: 20 */

    bge   x0, x0, mul_condition     /* pc: 24 */
mul_loop:
    add   x13, x13, x10     /* pc: 28 */
    addi  x12, x12, 1       /* pc: 32 */
mul_condition:
    blt   x12, x11, mul_loop        /* pc: 36 */
    addi  x12, x13, 0       /* pc: 40 */
    jalr   x0, x3, 0        /* pc: 44 */

