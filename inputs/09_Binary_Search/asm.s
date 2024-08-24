.section .rodata
input_format_int: .asciz "%d"
found_flag: .space 1
found: .asciz "The element was found on position %d!\n"
not_found: .asciz "The element was NOT found!\n"
size: .space 4
needle: .space 4
array: .space 256

.section .text
.global binsearch
binsearch:
    # a0 = int arr[]
    # a1 = int needle
    # a2 = int size
    # t0 = mid
    # t1 = left
    # t2 = right

    li      t1, 0        # left = 0
    addi    t2, a2, -1   # right = size - 1
1: # while loop
    bgt     t1, t2, 1f   # left > right, break
    add     t0, t1, t2   # mid = left + right
    srai    t0, t0, 1    # mid = (left + right) / 2

    # Get the element at the midpoint
    slli    t4, t0, 2    # Scale the midpoint by 4
    add     t4, a0, t4   # Get the memory address of arr[mid]
    lw      t4, 0(t4)    # Dereference arr[mid]

    # See if the needle (a1) > arr[mid] (t3)
    ble     a1, t4, 2f   # if needle <= t3, we need to check the next condition
    # If we get here, then the needle is > arr[mid]
    addi    t1, t0, 1    # left = mid + 1
    j       1b
2:
    bge     a1, t4, 2f   # skip if needle >= arr[mid]
    # If we get here, then needle < arr[mid]
    addi    t2, t0, -1   # right = mid - 1
    j       1b
2:
    # If we get here, then needle == arr[mid]
    mv      a0, t0
    # Update flag
    li      t0, 1
    la      t1, found_flag
    sb      t0, 0(t1)
    ret
1:
    li      t0, 0
    la      t1, found_flag
    sb      t0, 0(t1)
    ret

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
    beqz t0, find_elementf

    la a0, input_format_int
    mv a1, t1
    call scanf

    addi t0, t0, -1
    addi t1, t1, 4
    j read_arrayb

find_element:
    la a0, input_format_int
    la a1, needle
    call scanf

    la a0, array
    la a1, needle
    lw a1, 0(a1)
    la a2, size
    lw a2, 0(a2)
    call binsearch

    la t0, found_flag
    lb t1, 0(t0)
    beqz t1, not_found_elementf

    addi a1, a0, 1
    la a0, found
    call printf

    j exitf

not_found_element:
    la a0, not_found
    call printf

exit:
    ld ra, 0(sp)
    ret
