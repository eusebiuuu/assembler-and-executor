from instruction_name_counter import instruction_counter


with open('../utils/instruction_enum.hpp', 'w') as file:

    file.write('#pragma once\nenum class InstructionType {')
    
    for instruction, count in instruction_counter:
        file.write(f'\n\t{instruction.replace('.','_')},')
    
    file.write('\n\tNOT_IMPLEMENTED\n};')

with open('../utils/dumb_parser.hpp', 'w') as file:

    file.write('#pragma once\n#include "./instruction_enum.hpp"\n#include <string>\nInstructionType instruction_to_enum(const std::string& instruction){')
    
    for instruction, count in instruction_counter:
        file.write(f'\n\tif(instruction == "{instruction}"){{return InstructionType::{instruction.replace('.','_')};}}')

    file.write('\n\tthrow std::runtime_error("Failed to get enum for instruction: \'" + instruction + "\'");\n}')