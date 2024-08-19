#pragma once
#include "registers_enum.h"
#include "string"

RegisterIntType parseIntRegister(const std::string &reg) {
    if(reg == "zero"){return RegisterIntType::zero;}
	if(reg == "t0"){return RegisterIntType::t0;}
	if(reg == "t1"){return RegisterIntType::t1;}
	if(reg == "t2"){return RegisterIntType::t2;}
	if(reg == "t3"){return RegisterIntType::t3;}
	if(reg == "t4"){return RegisterIntType::t4;}
	if(reg == "t5"){return RegisterIntType::t5;}
	if(reg == "a0"){return RegisterIntType::a0;}
	if(reg == "a1"){return RegisterIntType::a1;}
	if(reg == "a2"){return RegisterIntType::a2;}
	if(reg == "a3"){return RegisterIntType::a3;}
	if(reg == "ra"){return RegisterIntType::ra;}
	if(reg == "sp"){return RegisterIntType::sp;}
	if(reg == "s1"){return RegisterIntType::s1;}
	return RegisterIntType::zero;
}

RegisterFloatType parseFloatRegister(const std::string &reg) {
    if(reg == "ft0"){return RegisterFloatType::ft0;}
    if(reg == "ft1"){return RegisterFloatType::ft1;}
    if(reg == "ft2"){return RegisterFloatType::ft2;}
    if(reg == "ft3"){return RegisterFloatType::ft3;}
	if(reg == "fa0"){return RegisterFloatType::fa0;}
	if(reg == "fa1"){return RegisterFloatType::fa1;}
	if(reg == "fa2"){return RegisterFloatType::fa2;}
	return RegisterFloatType::ft0;
}