.section .rodata
input_format: .asciz "%s"
output_format: .asciz "The string at the new address is %s.\n"
string: .space 256
new_address: .space 256

.section .text
.global stringcopy
stringcopy:
    # a0 = destination
    # a1 = source
1:
    lb      t0, 0(a1)    # Load a char from the src
    sb      t0, 0(a0)    # Store the value of the src
    beqz    t0, 1f       # Check if it's 0

    addi    a0, a0, 1    # Advance destination one byte
    addi    a1, a1, 1    # Advance source one byte
    j       1b           # Go back to the start of the loop
1:
    ret                  # Return back via the return address

.global main
main:
    sd ra, 0(sp)
    la a0, input_format
    la a1, string
    call scanf

    la a1, string
    la a0, new_address
    call stringcopy

    la a1, new_address
    la a0, output_format
    call printf

    ld ra, 0(sp)
    ret