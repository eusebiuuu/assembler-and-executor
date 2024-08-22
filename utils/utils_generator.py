import os

instruction_counter = {}

for folder in os.listdir('../inputs'):
    with open(f'../inputs/{folder}/asm.s') as file:
        for line in file.readlines():
            if not line.isspace() and line[0].isspace():
                word = line.split(maxsplit=1)[0]
                if word[0] != '#':
                    instruction_counter[word] = instruction_counter.get(word, 0) + 1


with open('../utils/instruction_enum.h', 'w') as file:

    file.write('#ifndef INSTRUCTION_ENUM\n#define INSTRUCTION_ENUM\nenum InstructionType {')
    first = True
    for instruction in instruction_counter.keys():
        if not first:
            file.write(',')
        file.write(f'''\n\t{instruction.replace('.','_')}''')
        first = False
    file.write('\n};\n#endif')

with open('../utils/dumb_parser.hpp', 'w') as file:

    file.write('#ifndef DUMB_PARSER\n#define DUMB_PARSER\n#include "./instruction_enum.h"\n#include <string>\nInstructionType instruction_to_enum(const std::string& instruction){')
    for instruction in instruction_counter.keys():
        file.write(f'''\n\tif(instruction == "{instruction}"){{return InstructionType::{instruction.replace('.','_')};}}''')
    file.write('\n}\n#endif')