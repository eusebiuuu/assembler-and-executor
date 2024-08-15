# Assembler-and-executor

1. Tasks
   1. Assembler
      1. Research
         1. [Research about RISC-V](https://riscv.org/wp-content/uploads/2017/05/riscv-spec-v2.2.pdf)
         2. <https://github.com/Proiect-ASC/riscv-emulator>
         3. <https://github.com/Stefan-101/RISC-Assembler-and-Interpreter>
         4. <https://github.com/Cosmin371/Assembler-and-executor>
         5. <https://github.com/Cris24dc/RISC-V-Assembler-and-Executor>
         6. <https://github.com/LeusteanAndrei/ProiectASC>
      2. Design the assembler workflow
            1. Lexical Analysis: Parse the input assembly code into tokens (e.g., instructions, registers, immediates)
               1. Create regex for each format
               2. Convert each register to the canonical name - chapter 20 from the official docs
            2. Syntax Analysis: Verify the structure of the instructions, ensuring they conform to the RISC-V assembly syntax.
            3. Symbol Table: Implement a symbol table to manage labels and resolve their addresses during assembly.
            4. Instruction Encoding: Convert each parsed instruction into its binary format according to the RISC-V specification.
            5. Output: Generate the machine code as a binary file or a hex format file.
      3. Testing
            1. Write simple RISC-V assembly programs and verify the output of your assembler by comparing it with the output from a known assembler like the GNU Assembler (as).
            2. <https://projectf.io/posts/riscv-cheat-sheet/>
      4. [RISC-V samples](https://marz.utk.edu/my-courses/cosc230/book/example-risc-v-assembly-programs/)
   2. Executor
      1. Implement a RISC-V CPU Model:
         1. Registers: Register file & implement an array or a similar structure to represent the 32 general-purpose registers.
         2. Program Counter (PC): Manage a program counter to keep track of the current instruction being executed.
         3. Memory Model: Implement a simple memory model to load and store data. This can be an array representing RAM.
         4. Every executed file has a state file that consists of registers values and memory
         5. State file at the end of execution
         6. Instruction Fetch-Decode-Execute Cycle: Implement the cycle where the executor fetches the instruction, decodes it, and performs the required operation.
         7. Instruction Handling: Write code to handle each type of RISC-V instruction, including arithmetic, logical, control flow, and memory instructions.
      2. Handle Exceptions: implement basic exception handling for situations like invalid instructions or memory access violations.
      3. I/O Handling (Optional): to make your executor more interactive, you could implement simple I/O mechanisms, such as printing results or reading from input.
      4. Testing: Create or use existing machine code binaries and verify the output by comparing it with an established RISC-V simulator like Spike or QEMU.
2. Optimise the speed and accuracy & Max 8kB of memory of binary buffer for both
3. Project description
   1. [Choose an open source licence](https://choosealicense.com)
   2. Overview
       1. Introduction
       2. Purpose
       3. General workflow
       4. Technologies and tools
       5. Code structure
       6. Technical decisions
       7. Performance
       8. Security
       9. Future implementations
   3. Functionalities
   4. Design
   5. Design principles
   6. Business logic & code snippets
   7. Project documentation
   8. Installation setup
