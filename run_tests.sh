#!/usr/bin/env bash

for dir in './inputs'/*/; do

    echo -e "\n\t$(basename "$dir"):\nassembler:"

    './outputs/parser' "${dir}asm.s" "./outputs/$(basename "$dir").bin" './encoder/encodings.txt'

    echo -e "\ninterpreter:"

    for file in "$dir/tests"/*; do

        echo -e "\n\t\t$(basename "$file"):"

        './outputs/interpreter' "./outputs/$(basename "$dir").bin" "./outputs/$(basename "$dir").out" './encoder/encodings.txt' './utils/int_registers.txt' './utils/float_registers.txt' < "$file"
    done

    echo ""

done