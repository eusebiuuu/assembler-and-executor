.section .rodata
input_format_int: .asciz "%d"
output_format: .asciz "The sum is %d.\n"
size: .space 4
array: .space 256

.section .text
.global arraysum
arraysum:
    # a0 = int a[]
    # a1 = int size
    # t0 = ret
    # t1 = i
    li    t0, 0        # ret = 0
    li    t1, 0        # i = 0
1:  # For loop
    bge   t1, a1, 1f   # if i >= size, break
    slli  t2, t1, 2    # Multiply i by 4 (1 << 2 = 4)
    add   t2, a0, t2   # Update memory address
    lw    t2, 0(t2)    # Dereference address to get integer
    add   t0, t0, t2   # Add integer value to ret
    addi  t1, t1, 1    # Increment the iterator
    j     1b           # Jump back to start of loop (1 backwards)
1:
    mv    a0, t0       # Move t0 (ret) into a0
    ret                # Return via return address register

.global main
main:
    sd ra, 0(sp)

    la a0, input_format_int
    la a1, size
    call scanf

    la a1, size
    lw t0, 0(a1)
    la t1, array

read_array:
    beqz t0, get_sumf

    la a0, input_format_int
    mv a1, t1
    call scanf

    addi t0, t0, -1
    addi t1, t1, 4
    j read_arrayb
get_sum:
    la a0, array
    la a1, size
    lw a1, 0(a1)
    mv t0, zero
    call arraysum

    la a0, output_format
    mv a1, t0
    call printf
exit:
    ld ra, 0(sp)
    ret