#!/usr/bin/env bash

function main() {
    for dir in './inputs'/*/; do

        local category_name="$(basename "$dir")"

        echo -e "\n\t\t$category_name:\nassembler:"

        './outputs/parser' "${dir}asm.s" "./outputs/$category_name.bin" './encoder/encodings.txt'

        echo -e "\ninterpreter:"

        for file in "$dir/tests"/*; do

            local test_file_name="$(basename "$file")"
            test_file_name="${test_file_name%.*}"

            echo -e "\n\t$test_file_name:"

            timeout 0.5s './outputs/interpreter' "./outputs/$category_name.bin" "./outputs/${category_name}_$test_file_name.out" './encoder/encodings.txt' './utils/int_registers.txt' './utils/float_registers.txt' < "$file"
        done

        echo ""

    done
}

main