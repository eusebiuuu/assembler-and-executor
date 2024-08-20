#ifndef REGISTERS_ENUM
#define REGISTERS_ENUM

enum RegisterIntType {
    zero = 0,
    t0,
    t1,
    t2,
    t3,
    t4,
    t5,
    a0,
    a1,
    a2,
    a3,
    sp,
    ra,
    s1,
};

enum RegisterFloatType {
    ft0,
    ft1,
    ft2,
    ft3,
    fa0,
    fa1,
    fa2,
};

int const INT_REGISTER_COUNT = RegisterIntType::s1 + 1;

int const FLOAT_REGISTER_COUNT = RegisterFloatType::fa2 + 1;

int const INT_REGISTER_BITS = 4;

int const FLOAT_REGISTER_BITS = 3;

#endif