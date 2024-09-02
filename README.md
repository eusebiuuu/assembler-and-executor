# RISC-V emulator
In this project, I intended to build an emulator for RISC-V assembly language, that is, a piece of software that assembles and executes a given RISC-V program.

## Table of contents
- [Overview](#overview)
- [Installation](#installation-setup-and-usage)
- [Functionalities](#functionalities)
- [Credits](#credits)
- [References](#references)

## Overview
This project implements the functionalities of a RISC-V emulator using C++, but with some limitations, among others that you can check in the [Functionalities](#functionalities) section, on the number of recognized instructions and registers. Also, the encodings used for instructions are generated automatically using the Huffman encoding algorithm given their frequency in the dataset (that you can consult in the [References](#references) section).

## Installation setup and usage
1. Clone the repository locally
```sh
git clone https://github.com/eusebiuuu/assembler-and-executor.git
```
2. Run the tests
```sh
make all test
```
3. You can add more tests by creating a directory in `inputs` folder that consists of the assembly program with the name of `asm.s` and a sub-directory with tests. For more information you can check the structure of the existing tests.


## Functionalities
##### General description
* First part of the emulator represents the encodings generation. Basically, we want to get the encodings for all instructions from the dataset in an efficient manner, so that the most frequent ones are shorter and also there are no ambiguities. This can be done pretty easily with Huffman encoding algorithm, given the frequencies of the instructions.

* Second part of the emulator is the parser. In order for the emulator to parse the assembly code properly, I divided the all possible statements into 3 categories: labels - these are addresses of variables or instructions used in assembly for a more modular written code, directives - these statements specify what type of text are the following lines and, also, its visibility to the linker, and instructions - that are the actual commands the CPU executes. The instructions are further composed of the name of instruction, the registers used and the numbers (called immediates) that represent offsets of addresses or values. With these in mind, the assembler simply parses the code and, depending on the statement and its format (in case of instructions), encodes it and write the bytes in a binary file. Also, an extra piece of information here: to handle the labels that points to addresses of instructions or simply variable names in case of forward or backward jumps or loads, an extra parsing is done prior to the main one, in which are computed the addresses of the labels using a `std::unordered_map`.

* Last part is the interpreter. This parses the binary, decodes it and executes the corresponding instructions with the provided registers and immediates. In some cases conversions are needed, to store the floats and doubles in IEEE 754 Floating Point convention as numbers of 32 and 64 bits, respectively. Also, due to the fact that I couldn't include the code from `printf` and `scanf` functions, I had to hardcode them, but, also, make them support a variety of data types. Thus, I concluded the best strategy is, for now, to support only format strings made of only one data type.

##### CPU specifications
* The memory is divided as follows: 1kB of heap memory that is made of 3B defining the jump instruction to the main label i.e. the actual start of the program and the rest of storage space for variables, 2kB of code memory and 1kB of stack. An important note here is that this numbers can be easily changed by modifying the appropriate constants' values from the `cpu_spec.cpp` file. Also, after running a program, the executor prints in the `state_file.out` file from `outputs` directory the stack content as well as the final values of registers. Designing the addressing scheme was particularly interesting, due to the binary compression I used: the code segment needs bit addressable memory, but the example programs use byte addressable memory for computing array offsets and stack pointer operations. Thus, I feature a hybrid memory scheme, where only the code segment is bit addressable, the heap and stack being byte addressable (and byte aligned).
* The registers are divided, for minimising the length of encodings, into 14 integer registers and 6 float registers.
* The byte alignment is made by adding a proper number of bits as padding after the whole instruction has been written.
* Variable can be only of type `.asciz` or `.space`.
* Regarding endianness, I used big-endian due to ease in implementation and understanding.

##### Limitations
* One limitation is in the number of instructions and registers. That is, if one wants to add another instruction it should run the counter and encoding generator again and update all the instruction sizes in enum and in emulator logic, too. In case of registers, if one wants to add some more, it should pay attention to the binary representation of them and update it if necessary all over the place.
* Other limitations are in the C functions (`printf` and `scanf`) that cannot handle multiple parameters at once.

##### Future enhancements
* Refactor the code structure within files and project structure
* Scanf and printf logic and proper use
* Generate the instructions sizes dynamically & add more (fsd, mul)
* Improve the parser and the interpreter for more general uses & handle exceptions
* Create a lexer
* Brainstorm other ideas

## Credits
Big thanks to [Pojoga Dragos](https://github.com/Dragos-Florin-Pojoga) for providing valuable help in terms of guidance, project structure and coding style.

## References
The encodings were generated based on [this](https://marz.utk.edu/my-courses/cosc230/book/example-risc-v-assembly-programs/) dataset.

