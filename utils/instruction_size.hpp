#pragma once
#include "./instruction_enum.hpp"
#include <string>

int get_instruction_size(InstructionType t) {
	switch(t) {
		case InstructionType::li: {return 2;}
		case InstructionType::addi: {return 3;}
		case InstructionType::bgt: {return 4;} // 4p
		case InstructionType::add: {return 2;}
		case InstructionType::srai: {return 3;}
		case InstructionType::slli: {return 2;}
		case InstructionType::lw: {return 3;}
		case InstructionType::ble: {return 4;} // 5p
		case InstructionType::j: {return 3;} // 7p
		case InstructionType::bge: {return 4;} // 7p
		case InstructionType::mv: {return 2;} // 3p
		case InstructionType::ret: {return 1;} // 4p
		case InstructionType::sd: {return 3;}
		case InstructionType::call: {return 3;} // 6p
		case InstructionType::sub: {return 3;} // 5p
		case InstructionType::lb: {return 2;}
		case InstructionType::sb: {return 2;}
		case InstructionType::ld: {return 3;}
		case InstructionType::fmv_s: {return 2;} // 5p
		case InstructionType::fgt_s: {return 2;}
		case InstructionType::beqz: {return 3;} // 3p
		case InstructionType::flt_s: {return 2;}
		case InstructionType::fsw: {return 2;}
		case InstructionType::fld: {return 2;}
		case InstructionType::fsub_d: {return 2;} // 1p
		case InstructionType::fmul_d: {return 2;} // 1p
		case InstructionType::fadd_d: {return 2;}
		case InstructionType::fsqrt_d: {return 2;} // 3p
		case InstructionType::la: {return 3;} // 1p
		case InstructionType::bnez: {return 3;}
		case InstructionType::fmv_s_x: {return 2;} // 2p
		//case InstructionType::flw: {return 2;}
		//case InstructionType::fmul_s: {return 2;}
		//case InstructionType::fadd_s: {return 2;}
		default: {throw std::runtime_error("Failed to get instruction size for instruction: '" + std::to_string((int)t) + "'");}
	}
}