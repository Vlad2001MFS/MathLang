#pragma once
#include <cstdint>

enum class OpCode : uint8_t {
    Jmp,
    Call,
    Ret,
    Add,
    Sub,
    Mul,
    Div,
    Push,
    Pop,
    Set,
    Get,
    Unset,
    Int,
    _Count
};

using LabelAddress = uint32_t;