.section .rodata
input_format_string: .asciz "%s"
input_format_int: .asciz "%d"
output_format: .asciz "The new string is %s.\n"
count: .space 4
string: .space 256
new_address: .space 256

.section .text
.global strncpy
strncpy:
    # a0 = char *dst
    # a1 = const char *src
    # a2 = unsigned long n
    # t0 = i
    li      t0, 0        # i = 0
1:  # first for loop
    bge     t0, a2, 1f   # break if i >= n
    add     t1, a1, t0   # src + i
    lb      t1, 0(t1)    # t1 = src[i]
    beqz    t1, 1f       # break if src[i] == '\0'
    add     t2, a0, t0   # t2 = dst + i
    sb      t1, 0(t2)    # dst[i] = src[i]
    addi    t0, t0, 1    # i++
    j       1b           # back to beginning of loop
1:  # second for loop
    bge     t0, a2, 1f   # break if i >= n
    add     t1, a0, t0   # t1 = dst + i
    sb      zero, 0(t1)  # dst[i] = 0
    addi    t0, t0, 1    # i++
    j       1b           # back to beginning of loop
1:
    # we don't have to move anything since
    # a0 hasn't changed.
    ret                  # return via return address register

.global main
main:
    sd ra, 0(sp)
    la a0, input_format_string
    la a1, string
    call scanf

    la a0, input_format_int
    la a1, count
    call scanf

    la a0, new_address
    la a1, string
    la t0, count
    lw a2, 0(t0)
    call strncpy

    la a1, new_address
    la a0, output_format
    call printf

    ld ra, 0(sp)
    ret