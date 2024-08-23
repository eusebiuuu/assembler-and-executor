#pragma once

#include "./registers_enum.hpp"

const int STACK_SIZE = 1000;
const int MEMORY_SIZE = 1000;
const int BINARY_SIZE = 2000;

const int TOTAL_MEMORY = STACK_SIZE + MEMORY_SIZE + BINARY_SIZE;

const int ADDRESS_SIZE = 13;


const int INVALID_FUNCTION_CALL = 0XFFF;

const int STACK_STEP = 8;

const int EXIT_ADDRESS = 0XFFF;
const int MAX_STRING_LEN = 256;



const int INT_REGISTER_COUNT = (int)RegisterIntType::NOT_IMPLEMENTED;
const int FLOAT_REGISTER_COUNT = (int)RegisterFloatType::NOT_IMPLEMENTED;

const int INT_REGISTER_BITS = 4;
const int FLOAT_REGISTER_BITS = 3;