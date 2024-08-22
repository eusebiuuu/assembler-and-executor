.section .rodata
enter_prompt: .asciz "Enter a, b, and c: "
scan: .asciz "%d"
result_out: .asciz "Result = %d\n"

.section .text
.global cfunc
cfunc:
    add a1, a1, a2
    add a0, a0, a1
    ret

.global main
main:
    addi    sp, sp, -32     # Allocate 32 bytes from the stack
    sd      ra, 0(sp)       # Since we are making calls, we need the original ra

    # Prompt the user first
    la      a0, enter_prompt 
    call    printf

    # We've printed the prompt, now wait for user input
    la      a0, scan
    addi    a1, sp, 8
    call    scanf

    la      a0, scan
    addi    a1, sp, 16
    call    scanf

    la      a0, scan
    addi    a1, sp, 24
    call    scanf

    # Now all of the values are in memory, load them
    # so we can jal ra, the c function.
    lw      a0, 8(sp)
    lw      a1, 16(sp)
    lw      a2, 24(sp)
    call    cfunc

    # The result should be in a0, but that needs to be
    # the second parameter to printf.
    mv      a1, a0
    la      a0, result_out
    call    printf

    # Restore original RA and return
    ld      ra, 0(sp)
    addi    sp, sp, 32       # Always deallocate the stack!
    ret