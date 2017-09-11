//
// Created by dimitrije on 9/10/17.
//

#ifndef EMULATOR_M0_ENCODER_H
#define EMULATOR_M0_ENCODER_H

#include <cstdint>
#include <array>
#include "conditions.h"
#include "../cpu/registers.h"

typedef int16_t instruction;

class encoder {

    /**
     * ADCS R0-R7, R0-R7, R0-R7 - Rd and Rn must specify the same register.
     * @param Rd The result register
     * @param Rn The first source register
     * @param Rm the second source register
     * @return binary encoded value | 0 1 0 0 0 0 | 0 1 0 1 | Rm | Rd |
     */
    int16_t addWithCarry(registers Rd, registers Rn, registers Rm);

    /**
     * ADD <Rd>, <Rm> - adds the values of two registers, one or both of which are high registers
     * this instruction does not change the flags.
     * ADD <Rd>, <Rn>, <Rm> - adds the value of one register to the value of a second register,
     * and stores the result in a third register. It updates the condition code flags, based on the result (ADDS arm instruction)
     * @param Rd - The result register
     * @param Rn - The first source register
     * @param Rm - The second source register
     * If the Rd and Rm are high registers (R8-R15) the Rd and Rn register have to be the same
     * @return ADD <Rd>, <Rn>, <Rm> --> |0 0 0 1 1 0 0 | Rm | Rn | Rd |
     *         ADD <Rd>, <Rm>       --> |0 1 0 0 0 1 0 0 | H1 | H2 | Rm | Rd | (H1 and H2 are the highest bits)
     */
    int16_t add(registers Rd, registers Rn, registers Rm, bool S);


    /**
     * ADD <Rd>, <Rn>, #<immed_3>
     * It updates the condition code flags, based on the result.
     * @param Rd - is the destination register for the completed operation. (R0-R7)
     * @param Rn - specifies the register that contains the operand for the addition.(R0-R7)
     * @param immed - specifies a 3-bit immediate value that is added to the value of <Rn>
     * @return | 0 0 0 1 1 1 0 | immed_3 | Rn | Rd |
     */
    int16_t add(registers Rd, registers Rn, char immed);

    /**
     * ADD <Rd>, #<immed_8>
     * It updates the condition code flags, based on the result.
     * @param Rd - Holds the first operand for the addition, and is the destination register for the
     * completed operation.
     * @param immed - Specifies an 8-bit immediate value that is added to the value of <Rd>
     * @return | 0 0 1 1 0 | Rd | immed_8 |
     */
    int16_t add(registers Rd, char immed);

    /**
     * ADD <Rd>, PC, #<immed_8> * 4
     * PC indicates PC-relative addressing
     * The condition codes are not affected.
     * @param Rd - Is the destination register for the completed operation.
     * @param immed - Specifies an 8-bit immediate value that is quadrupled and added to the value of the PC.
     * @return | 1 0 1 0 0 | Rd | immed_8 |
     */
    int16_t addPC(registers Rd, char immed);

    /**
     * ADD <Rd>, SP, #<immed_8> * 4
     * SP indicates SP-relative addressing
     * The condition codes are not affected.
     * @param Rd - Is the destination register for the completed operation.
     * @param immed - Specifies an 8-bit immediate value that is quadrupled and added to the value of the SP.
     * @return | 1 0 1 0 1 | Rd | immed_8 |
     */
    int16_t addSP(registers Rd, char immed);

    /**
     * ADD SP, #<immed_7> * 4
     * SP Contains the first operand for the addition. SP is also the destination register for the
     * operation.
     * The condition codes are not affected.
     * @param immed Specifies the immediate value that is quadrupled and added to the value of the SP
     * @return | 1 0 1 1 0 0 0 0 0 | immed_7 |
     */
    int16_t addToSP(char immed);

    /**
     * AND <Rd>, <Rm>
     * AND (Logical AND) performs a bitwise AND of the values in two registers.
     * updates the condition code flags, based on the result.
     * @param Rd - Specifies the register containing the first operand, and is also the destination register.
     * @param Rm - Specifies the register containing the second operand.
     * @return | 0 1 0 0 0 0 0 0 0 0 | Rm | Rd |
     */
    int16_t andOp(registers Rd, registers Rm);

    /**
     * ASR <Rd>, <Rm>, #<immed_5>
     * Arithmetic - Shift Right provides the signed value of the contents of a register divided by a power of 2.
     * It updates the condition code flags, based on the result.
     * @param - Rd Is the destination register for the completed operation.
     * @param - Rm Specifies the register that contains the value to be shifted.
     * @param - immed Specifies the shift amount, in the range 1 to 32. Shifts by 1 to 31 are encoded directly in
     * immed_5. A shift by 32 is encoded as immed_5 == 0.
     * @return | 0 0 0 1 0 | immed_5 | Rm | Rd |
     */
    int16_t asrOp(registers Rd, registers Rm, int8_t immed);

    /**
     * ASR <Rd>, <Rs>
     * provides the signed value of the contents of a register divided by a variable power of 2.
     * It updates the condition code flags, based on the result.
     * @param Rd - Contains the value to be shifted, and is also the destination register for the completed operation.
     * @param Rs - Specifies the register that contains the value of the shift
     * @return | 0 1 0 0 0 0 0 1 0 0 | Rs | Rd |
     */
    int16_t asrOp(registers Rd, registers Rs);

    /**
     * B<cond> <target_address>
     * @param cond - Is the condition under which the instruction is executed.
     * @param target_address - Specifies the address to branch to. The branch target address is calculated by:
     * 1. Shifting the 8-bit signed offset field of the instruction left by one bit.
     * 2. Sign-extending the result to 32 bits.
     * 3. Adding this to the contents of the PC (which contains the address of the branch instruction plus 4).
     * @return | 1 1 0 1 | cond | signed_immed_8 |
     */
    int16_t bOp(conditions cond, int8_t target_address);

    /**
     * B <target_address>
     * @param target_address - Specifies the address to branch to. The branch target address is calculated by:
     * 1. Shifting the 11-bit signed offset of the instruction left one bit.
     * 2. Sign-extending the result to 32 bits.
     * 3. Adding this to the contents of the PC (which contains the address of the branch instruction plus 4).
     * @return | 1 1 1 0 0 | signed_immed_11 |
     */
    int16_t bOp(int16_t target_address);

    /**
     * BIC <Rd>, <Rm>
     * BIC updates the condition code flags, based on the result.
     * @param Rd - Is the register containing the value to be ANDed, and is also the destination register for the
     * completed operation.
     * @param Rm - Specifies the register that contains the value whose complement is ANDed with the value in <Rd>.
     * @return | 0 1 0 0 0 0 1 1 1 0 | Rm | Rd |
     */
    int16_t bicOp(registers Rd, registers Rm);

    /**
     * BKPT <immed_8>
     * BKPT (Breakpoint) causes a software breakpoint to occur. This breakpoint can be handled by an exception
     * handler installed on the Prefetch Abort vector. In implementations which also include debug hardware, the
     * hardware can optionally override this behavior and handle the breakpoint itself. When this occurs, the
     * Prefetch Abort vector is not entered
     * @param immed - Is an 8-bit immediate value, which is placed in bits[7:0] of the instruction. This value is
     * ignored by the ARM hardware, but can be used by a debugger to store additional information about the breakpoint.
     * @return | 1 0 1 1 1 1 1 0 | immed_8 |
     */
    int16_t bkptOp(int8_t immed);


    /**
     * BL{X} (Branch with Link) provides an unconditional subroutine call to another Thumb routine.
     * @param address -2^(22) <= offset <= +2^(22) - 2
     * @param exchange true if BLX and false for BL
     * @return generates two instructions of the format  | 1 1 1 | H | offset_11 |
     * where the first instruction has H = 0b10 and the H = 0b11 for BL and H = 0b01
     */
    std::array<int16_t, 2> blxOp(int32_t address, bool exchange);

    int16_t

};


#endif //EMULATOR_M0_ENCODER_H
