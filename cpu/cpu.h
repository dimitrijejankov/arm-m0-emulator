//
// Created by dimitrije on 9/11/17.
//

#ifndef EMULATOR_M0_DECODER_H
#define EMULATOR_M0_DECODER_H


#include <cstdint>

class cpu {

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 0 0 | Op | Offset5 | Rs | Rd |
     *
     * Op is a 2 bit value
     * Offset5 is a 5 bit value
     * Rs is a 3 bit value
     * Rd is a 3 bit value
     *
     * @param instr 16 bit instruction
     */
    void move_shifted_register(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 0 0 1 1 | I | Op | Rn/offset3 | Rs | Rd |
     *
     * I - 1 if immediate value, 0 otherwise
     * Op - 2 bit operation
     * Rn/offset3 - if I is 1 it is a 3 bit offset a register otherwise
     * @param instr 16 bit instruction
     */
    void add_subtract(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 0 1 | Op | Rd | Offset8 |
     *
     * Op - 2 bit operation
     * Rd - the 3 bit register
     * Offset8 - an 8 bit offset
     *
     * @param instr 16 bit instruction
     */
    void move_compare_add_subtract_immediate(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 0 0 0 | Op | Rs | Rd |
     *
     * Op - 3 bit operation
     * Rs - 3 bit register
     * Rd - 3 bit register
     *
     * @param instr 16 bit instruction
     */
    void alu_operations(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 0 0 1 | Op | H1 | H2 | Rs/Hs | Rd/Hd |
     *
     * Op - 3 bit operation (op 0b11 is branch exchange)
     * H1 - 1 if the fist register is a high register 0 otherwise
     * H2 - 1 if the second register is a high register 0 otherwise
     * Rs/Hs - three least significant bits of the high or low register
     * Rd/Hd - three least significant bits of the high or low
     * @param instr
     */
    void hi_register_operations_branch_exchange(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 0 1 | Rd | Word8 |
     *
     * Rd - three bits of a low register
     * Word8 - the 8 bit word
     * @param instr
     */
    void pc_relative_load(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 1 | L | B | 0 | Ro | Rb | Rd |
     *
     * L  - 1 if we are transferring a byte quantity, 0 if we are transferring a word
     * B  - 1 if we are loading from memory, 0 if we are storing to memory
     * Ro - offset register, least significant 3 bit of a low register
     * Rb - base register, least significant 3 bit of a low register
     * Rd - source/destination register, least significant 3 bit of a low register
     * @param instr
     */
    void load_store_with_register_offset(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 1 | H | S | 1 | Ro | Rb | Rd |
     *
     * H  - used to with the S flag to specify what load instruction it is used
     * S  - 0 if the operand not sign-extended, 1 if it is
     * Ro - offset register, least significant 3 bit of a low register
     * Rb - base register, least significant 3 bit of a low register
     * Rd - source/destination register, least significant 3 bit of a low register
     * @param instr
     */
    void load_store_sign_extended_byte_halfword(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 1 | B | L | Offset5 | Rb | Rd |
     *
     * B - byte word flag 1 to transfer a byte quantity 0 to transfer a word
     * L - 1 to load the memory, 0 to store the memory
     * Offset5 - 5 bit Offset value
     * Rb - base register, least significant 3 bit of a low register
     * Rd - source/destination register, least significant 3 bit of a low register
     * @param instr
     */
    void load_store_with_immediate_offset(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 0 0 | L | Offset5 | Rb | Rd |
     *
     * L - 0 to store to memory, 1 to load from the memory
     * Offset5 - 5 bit offset
     * Rb - base register, least significant 3 bit of a low register
     * Rd - destination register, least significant 3 bit of a low register
     * @param instr
     */
    void load_store_halfword_immediate_offset(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 0 1 | L | Rd | Word8 |
     *
     * L - 0 to store to memory, 1 to load from the memory
     * Rd - destination register, least significant 3 bit of a low register
     * Word8 - Immediate value
     *
     * @param instr
     */
    void sp_relative_load_store(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 1 0 | SP | Rd | Word8 |
     *
     * SP - source register 1 if SP, 0 if PC
     * Rd - destination register, least significant 3 bit of a low register
     * Word8 - 8-bit unsigned constant
     *
     * @param instr
     */
    void load_address(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 1 1 0 0 0 0 | S  | SWord7 |
     *
     * 0 - Offset is positive 1 - Offset is negative
     * SWord7 - 7-bit immediate value
     *
     * @param instr
     */
    void add_offset_to_stack_pointer(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 1 1 | L | 1 0 | R | Rlist |
     *
     * L - Load/Store bit 0 to store to memory, 1 to load from the memory
     * R - PC/LR bit 0 - Do not store LR/load PC, 1 - Store LR/Load PC
     * Rlist - 7 bits the least significant bit is R0 the most significant bit is R7 [R7, ..., R0]
     * @param instr
     */
    void push_pop_registers(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 0 | L | Rb | Rlist |
     *
     * L - Load/Store bit 0 if storing to memory, 1 if loading from memory
     * Rb - base register
     * Rlist - 7 bits the least significant bit is R0 the most significant bit is R7 [R7, ..., R0]
     *
     * @param instr
     */
    void multiple_load_store(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 1 | Cond | Soffset8 |
     *
     * Cond - 4 bit condition code
     * Soffset8 - 8-bit signed immediate
     * @param instr
     */
    void conditional_branch(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 1 1 1 1 1 | Value8 |
     *
     * Value8 - Comment field
     * @param instr
     */
    void software_interrupt(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 1 0 0 | Offset11 |
     *
     * Offset11 - 11 bits
     * @param instr
     */
    void unconditional_branch(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 1 1 | H | Offset |
     *
     * H - Low/high offset bit
     * Offset - 11 bit long branch and link offset high/low
     *
     * @param instr
     */
    void long_branch_with_link(uint16_t instr);
};


#endif //EMULATOR_M0_DECODER_H
