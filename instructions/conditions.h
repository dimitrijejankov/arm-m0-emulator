//
// Created by dimitrije on 9/9/17.
//

#ifndef EMULATOR_M0_CONDITIONS_H
#define EMULATOR_M0_CONDITIONS_H

/**
 * The number of conditions
 */
const int NUMBER_OF_CONDITIONS = 16;

/**
 * The opcode enum to index
 */
enum conditions {
    EQ = 0,
    NE = 1,
    CS_HS = 2,
    CC_LO = 3,
    MI = 4,
    PL = 5,
    VS = 6,
    VC = 7,
    HI = 8,
    LS = 9,
    GE = 10,
    LT = 11,
    GT = 12,
    LE = 13,
    AL = 14,
    UNPREDICTABLE = 15
};


/**
 * The opcode bytes
 */
const char conditionsBytes[] {
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

#endif //EMULATOR_M0_CONDITIONS_H
