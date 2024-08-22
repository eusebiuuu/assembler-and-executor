.section .rodata
input_format: .asciz "%s"
output_format: .asciz "The length of the input string is %d.\n"
store: .space 256

.section .text
strlen:
    # a0 = const char *str
    li     t0, 0         # i = 0
1: # Start of for loop
    add    t1, t0, a0    # Add the byte offset for str[i]
    lb     t1, 0(t1)     # Dereference str[i]
    beqz   t1, 1f        # if str[i] == 0, break for loop
    addi   t0, t0, 1     # Add 1 to our iterator
    j      1b            # Jump back to condition (1 backwards)
1: # End of for loop
    mv     a0, t0        # Move t0 into a0 to return
    ret                  # Return back via the return address register

.global main
main:
    sd ra, 0(sp)
    la a0, input_format
    la a1, store
    call scanf

    la a0, store
    call strlen

    mv a1, a0
    la a0, output_format
    call printf

    ld ra, 0(sp)
    ret