from instruction_name_counter import instruction_counter


with open('../utils/instruction_enum.h', 'w') as file:

    file.write('#ifndef INSTRUCTION_ENUM\n#define INSTRUCTION_ENUM\nenum InstructionType {')
    first = True
    for instruction, count in instruction_counter:
        if not first:
            file.write(',')
        file.write(f'''\n\t{instruction.replace('.','_')}''')
        first = False
    file.write('\n};\n#endif')

with open('../utils/dumb_parser.hpp', 'w') as file:

    file.write('#ifndef DUMB_PARSER\n#define DUMB_PARSER\n#include "./instruction_enum.h"\n#include <string>\nInstructionType instruction_to_enum(const std::string& instruction){')
    for instruction, count in instruction_counter:
        file.write(f'''\n\tif(instruction == "{instruction}"){{return InstructionType::{instruction.replace('.','_')};}}''')
    file.write('\n}\n#endif')