.section .rodata
input_format: .asciz "%f"
output_format_min: .asciz "Minimum is: %f\n"
output_format_max: .asciz "Maximum is: %f\n"
num_1: .space 4
num_2: .space 4
num_3: .space 4

.section .text
.global minmax
minmax:
    # fa0 = float a
    # fa1 = float b
    # fa2 = float c
    # ft0 = min
    # ft1 = max
    # a0  = float &mn
    # a1  = float &mx

    # Set mn = mx = a
    fmv.s   ft0, fa0
    fmv.s   ft1, fa0

    fgt.s   t0, ft0, fa1  # if (mn > b)
    beqz    t0, 1f        # skip if false
    # if we get here, then mn is > b
    fmv.s   ft0, fa1      # ft0 is mn, set it to b
    j       2f            # jump past the else if statement
1:
    flt.s   t0, ft1, fa1  # else if (mx < b)
    beqz    t0, 2f        # skip if false
    # if we get here then mx < b
    fmv.s   ft1, fa1      # ft1 is mx, set it to b
2:
    fgt.s   t0, ft0, fa2  # if (mn > c)
    beqz    t0, 1f        # skip if false
    # if we get here then mn > c
    fmv.s   ft0, fa2      # ft0 is mn, set it to c
    j       2f            # skip the else if statement
1:
    flt.s   t0, ft1, fa2  # else if (mx < c)
    beqz    t0, 2f        # skip if false
    # If we get here then mx < c
    fmv.s   ft1, fa2      # ft1 is mx, set it to c
2:
    fsw     ft0, 0(a0)    # store minimum into &mn
    fsw     ft1, 0(a1)    # store maximum into &mx
    ret                   # return via return address register

main:
    sd ra, 0(sp)

    la a0, input_format
    la a1, num_1
    call scanf

    la a0, input_format
    la a1, num_2
    call scanf

    la a0, input_format
    la a1, num_3
    call scanf

    la a0, num_1
    lw a0, 0(a0)
    fmv.s.x fa0, a0

    la a0, num_2
    lw a0, 0(a0)
    fmv.s.x fa1, a0

    la a0, num_3
    lw a0, 0(a0)
    fmv.s.x fa2, a0

    call minmax

    mv t0, a0
    mv t1, a1

    la a0, output_format_min
    lw a1, 0(t0)
    call printf

    la a0, output_format_max
    lw a1, 0(t1)
    call printf

    ld ra, 0(sp)
    ret