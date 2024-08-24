.section .rodata
input_format: .asciz "%lf"
output_format: .asciz "Distance: %lf\n"
point_1: .space 16
point_2: .space 16

.section .text
.global distance
distance:
    # a0 = Coordinate &from
    # a1 = Coordinate &to
    # Coordinate structure
    # Name    Offset     Size (bytes)
    # x       0          8
    # y       8          8
    # Total size = 16 bytes
    fld   ft0, 0(a0)      # ft0 = from.x
    fld   ft1, 8(a0)      # ft1 = from.y
    fld   ft2, 0(a1)      # ft2 = to.x
    fld   ft3, 8(a1)      # ft3 = to.y

    fsub.d  ft0, ft2, ft0 # ft0 = to.x - from.x
    fsub.d  ft1, ft3, ft1 # ft1 = to.y - from.y
    fmul.d  ft0, ft0, ft0 # ft0 = ft0 * ft0
    fmul.d  ft1, ft1, ft1 # ft1 = ft1 * ft1
    fadd.d  ft0, ft0, ft1 # ft0 = ft0 + ft1
    fsqrt.d fa0, ft0      # fa0 = sqrt(ft0)
    # Return value goes in fa0
    ret                   # Return

main:
    sd ra, 0(sp)

    la a0, input_format
    la a1, point_1
    call scanf

    la a0, input_format
    la a1, point_1
    addi a1, a1, 8
    call scanf

    la a0, input_format
    la a1, point_2
    call scanf

    la a0, input_format
    la a1, point_2
    addi a1, a1, 8
    call scanf

    la a0, point_1
    la a1, point_2
    call distance

    addi sp, sp, -8
    fsw fa0, 0(sp) # this won't work since we need to store 8 bytes, not 4 and we lack a proper command, like fsd...
    ld a1, 0(sp)
    addi sp, sp, 8
    
    la a0, output_format
    call printf

    ld ra, 0(sp)
    ret