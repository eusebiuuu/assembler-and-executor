#pragma once

enum class RegisterIntType {
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
    NOT_IMPLEMENTED
};

enum class RegisterFloatType {
    ft0,
    ft1,
    ft2,
    ft3,
    fa0,
    fa1,
    fa2,
    NOT_IMPLEMENTED
};