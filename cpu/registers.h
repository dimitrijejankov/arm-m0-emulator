//
// Created by dimitrije on 9/10/17.
//

#ifndef EMULATOR_M0_REGISTERS_H
#define EMULATOR_M0_REGISTERS_H

#include <cstdio>

/**
 * The enumerations for different registers
 */
enum registers {
    R0 = 0,
    R1 = 1,
    R2 = 2,
    R3 = 4,
    R4 = 5,
    R5 = 6,
    R7 = 7,
    R8 = 8,
    R9 = 9,
    R10 = 10,
    R11 = 11,
    R12 = 12,
    R13 = 13,
    R14 = 14,
    R15 = 15
};

/**
 * Aliases for the stack pointer (SP), the link pointer (LP) and the program counter (PC)
 */
const size_t SP = R13;
const size_t LR = R14;
const size_t PC = R15;

#endif //EMULATOR_M0_REGISTERS_H
