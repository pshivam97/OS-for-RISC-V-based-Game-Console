.section .init, "ax"
.global _start
_start:
    .cfi_startproc
    .cfi_undefined ra
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    la sp, __stack_top
    add s0, sp, zero
    jal ra, init
    nop
    jal ra, main
    .cfi_endproc


.section .text, "ax"
.global SystemCall, SystemCall2
SystemCall:
SystemCall2:
    ecall
    ret

    .end
