#ifndef DUMB_PARSER
#define DUMB_PARSER
#include "./instruction_enum.h"
#include <string>
InstructionType instruction_to_enum(const std::string& instruction){
	if(instruction == "li"){return InstructionType::li;}
	if(instruction == "addi"){return InstructionType::addi;}
	if(instruction == "bgt"){return InstructionType::bgt;}
	if(instruction == "add"){return InstructionType::add;}
	if(instruction == "srai"){return InstructionType::srai;}
	if(instruction == "slli"){return InstructionType::slli;}
	if(instruction == "lw"){return InstructionType::lw;}
	if(instruction == "ble"){return InstructionType::ble;}
	if(instruction == "j"){return InstructionType::j;}
	if(instruction == "bge"){return InstructionType::bge;}
	if(instruction == "mv"){return InstructionType::mv;}
	if(instruction == "ret"){return InstructionType::ret;}
	if(instruction == "sd"){return InstructionType::sd;}
	if(instruction == "call"){return InstructionType::call;}
	if(instruction == "sub"){return InstructionType::sub;}
	if(instruction == "lb"){return InstructionType::lb;}
	if(instruction == "sb"){return InstructionType::sb;}
	if(instruction == "ld"){return InstructionType::ld;}
	if(instruction == "fmv.s"){return InstructionType::fmv_s;}
	if(instruction == "fgt.s"){return InstructionType::fgt_s;}
	if(instruction == "beqz"){return InstructionType::beqz;}
	if(instruction == "flt.s"){return InstructionType::flt_s;}
	if(instruction == "fsw"){return InstructionType::fsw;}
	if(instruction == "fld"){return InstructionType::fld;}
	if(instruction == "fsub.d"){return InstructionType::fsub_d;}
	if(instruction == "fmul.d"){return InstructionType::fmul_d;}
	if(instruction == "fadd.d"){return InstructionType::fadd_d;}
	if(instruction == "fsqrt.d"){return InstructionType::fsqrt_d;}
	if(instruction == "la"){return InstructionType::la;}
	if(instruction == "bnez"){return InstructionType::bnez;}
	if(instruction == "fmv.s.x"){return InstructionType::fmv_s_x;}
	if(instruction == "flw"){return InstructionType::flw;}
	if(instruction == "fmul.s"){return InstructionType::fmul_s;}
	if(instruction == "fadd.s"){return InstructionType::fadd_s;}
	return InstructionType::ret;
}
#endif