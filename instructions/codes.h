//
// Created by dimitrije on 9/9/17.
//

#include <string>
#include <map>

#ifndef EMULATOR_M0_CODES_H
#define EMULATOR_M0_CODES_H

/**
 * The number of opcodes
 */
const int NUMBER_OF_OPCODES = 16;

/**
 * The opcode enum to index
 */
enum opcodes {
    AND = 0,
    EOR = 1,
    SUB = 2,
    RSB = 3,
    ADD = 4,
    ADC = 5,
    SBC = 6,
    RSC = 7,
    TST = 8,
    TEQ = 9,
    CMP = 10,
    CMN = 11,
    ORR = 12,
    MOV = 13,
    BIC = 14,
    MVN = 15
};

/**
 * The opcode bytes
 */
const char opcodeBytes[] {
        0b0000,
        0b0001,
        0b0010,
        0b0011,
        0b0100,
        0b0101,
        0b0110,
        0b0111,
        0b1000,
        0b1001,
        0b1010,
        0b1011,
        0b1100,
        0b1101,
        0b1110,
        0b1111
};

/**
 * The names of the instructions
 */
const std::string opcodeNames[] = {
        "AND",
        "EOR",
        "SUB",
        "RSB",
        "ADD",
        "ADC",
        "SBC",
        "RSC",
        "TST",
        "TEQ",
        "CMP",
        "CMN",
        "ORR",
        "MOV",
        "BIC",
        "MVN"
};

/**
 * Associates an index to the opcode
 */
const std::map<std::string, size_t> opcodeNameToIndex = {
        {"AND", AND},
        {"EOR", EOR},
        {"SUB", SUB},
        {"RSB", RSB},
        {"ADD", ADD},
        {"ADC", ADC},
        {"SBC", SBC},
        {"RSC", RSC},
        {"TST", TST},
        {"TEQ", TEQ},
        {"CMP", CMP},
        {"CMN", CMN},
        {"ORR", ORR},
        {"MOV", MOV},
        {"BIC", BIC},
        {"MVN", MVN}
};

#endif //EMULATOR_M0_CODES_H
