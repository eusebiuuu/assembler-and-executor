.section .rodata
string: .asciz "Hello world!"

.section .text
.global main
main:
    la a0, string
    call strlen
    ret # returning from main will exit the program