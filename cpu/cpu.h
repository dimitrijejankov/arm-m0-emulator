//
// Created by dimitrije on 9/11/17.
//

#ifndef EMULATOR_M0_DECODER_H
#define EMULATOR_M0_DECODER_H

#include <cstdint>
#include <vector>
#include "registers.h"
#include "../pheripherals/peripheral.h"
#include "mmu.h"

enum mode {
    THREAD_MODE,
    HANDLER_MODE
};

struct psr {

    /**
     * Exception number
     */
    uint8_t exception_number;

    /**
     * Thumb state bit
     */
    bool t;

    /**
     * Overflow flag
     */
    bool v;

    /**
     * Carry or borrow flag
     */
    bool c;

    /**
     * Zero flag
     */
    bool z;

    /**
     * Negative flag
     */
    bool n;
};

/**
 * The register type
 */
union arm_register_t {

  /**
   * Used to get the bytes (assuming little endian)
   */
  struct
  {
    uint8_t B0;
    uint8_t B1;
    uint8_t B2;
    uint8_t B3;

  } to_bytes;

  /**
   * Used to get the 16 words (assuming little endian)
   */
  struct
  {
    uint16_t W0;
    uint16_t W1;

  } to_half_words;

  /**
   * Used to get the unsigned 32-bit word
   */
  uint32_t to_uint;
};

class cpu {

private:

    const uint8_t REGISTER_MASK = 0b0000000000000111;
    const uint8_t OFFSET_5_MASK = 0b0000000000011111;
    const uint8_t OFFSET_8_MASK = 0b0000000011111111;
    const uint8_t OPERATION_2_MASK = 0b0000000000000011;
    const uint8_t OPERATION_3_MASK = 0b0000000000000111;
    const uint8_t FLAG_MASK = 0b0000000000000001;
    const uint8_t FLAG_MASK_2 = 0b0000000000000011;
    const uint8_t FLAG_MASK_4 = 0b0000000000001111;

    /**
     * This table is used to quickly figure out how many registers are used in an instruction like : ex. PUSH { Rlist }
     * it maps the bit field that selects the registers to a number
     */
    int cpu_bits_set[256];

    /**
     * returns the input value as a signed value
     * @param value - the value we want to have a signed reference
     * @return the signed reference
     */
    inline int32_t& to_signed(uint32_t &value) { return *((int32_t*)&value); }

    /**
     * returns the input value as a unsigned value
     * @param value - the value we want to have a signed reference
     * @return the signed reference
     */
    inline uint32_t& to_unsigned(int32_t &value) { return *((uint32_t*)&value); }

    /**
     * The current cpu mode can be either THREAD_MODE or HANDLER_MODE
     *
     * THREAD_MODE  - is used to execute application software.
     * The processor enters Thread mode when it comes out of reset.
     *
     * HANDLER_MODE - is used to handle exceptions.
     * The processor returns to Thread mode when it has finished all exception processing.
     */
    mode current_mode;

    /**
     * ARM M0 has 15 registers
     *
     * Registers R0-R7 are the low general purpose registers
     *
     * Registers R7-R15 are the the high general purpose registers
     *
     * Register R13 is the stack pointer - The main stack pointer (MSP) used in HANDLER_MODE or if set in the
     * CONTROL_REGISTER during the THREAD_MODE. The processor stack (PSP) used only during the THREAD_MODE if
     * set in the CONTROL_REGISTER.
     *
     * Register R14 is the link register (LR)
     *
     * Register R15 is the program counter (PC)
     */
    arm_register_t registers[16];

    /**
     * The pre-fetched instructions
     */
    uint16_t cpu_prefetch[2];

    /**
     * Flag to indicate if the processor currently is in hold mode.
     */
    bool holdState;

    /**
     * The next instruction we need to prefetch
     */
    uint32_t next_pc;

    /**
     * Program Status Register (PSR)
     */
    psr psr_register;

    /**
     * Inactive stack pointer - used to swap between the PSP and MSP this values is used for the stack pointer that is
     * currently inactive
     */
    uint32_t isp;

    /**
     * The mmu (used to get all the instructions and the data)
     */
    mmu *mmu_ptr;

    /**
     * Initializes the cpu bits set - this is used to figure out how many registers are selected
     */
    void init_cpu_bits_set() {
        for (int i = 0; i < 256; i++) {
            int count = 0;
            for (int j = 0; j < 8; j++){
                if (i & (1 << j)) {
                    count++;
                }
            }
            cpu_bits_set[i] = count;
        }
    }

    /**
     * Initializes the cpu to the state it is supposed to boot up
     */
    void reset();

    /**
     * Run the processor
     */
    void run();

    /**
     * Execute the operation
     */
    void execute_op(uint16_t instruction);

    /**
     * Prefetch does a prefetch based on the programming counter
     */
    void prefetch();

    /**
     * Prefetches the next instruction
     */
    void prefetch_next();

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
     * Op - 1 bit operation
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
     * @param instr - the instruction 
     */
    void hi_register_operations_branch_exchange(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 0 1 0 0 1 | Rd | Word8 |
     *
     * Rd - three bits of a low register
     * Word8 - the 8 bit word
     * @param instr - the instruction 
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
     * @param instr - the instruction 
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
     * @param instr - the instruction 
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
     * @param instr - the instruction 
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
     * @param instr - the instruction 
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
     * @param instr - the instruction 
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
     * @param instr - the instruction 
     */
    void load_address(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 1 1 0 0 0 0 | S  | SWord7 |
     *
     * 0 - Offset is positive 1 - Offset is negative
     * SWord7 - 7-bit immediate value
     *
     * @param instr - the instruction 
     */
    void add_offset_to_stack_pointer(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 0 1 1 | L | 1 0 | R | Rlist |
     *
     * L - Load/Store bit 0 to store to memory, 1 to load from the memory
     * R - PC/LR bit 0 - Do not store LR/load PC, 1 - Store LR/Load PC
     * Rlist - 8 bits the least significant bit is R0 the most significant bit is R7 [R7, ..., R0]
     * @param instr - the instruction 
     */
    void push_pop_registers(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 0 | L | Rb | Rlist |
     *
     * L - Load/Store bit 0 if storing to memory, 1 if loading from memory
     * Rb - base register
     * Rlist - 8 bits the least significant bit is R0 the most significant bit is R7 [R7, ..., R0]
     *
     * @param instr - the instruction 
     */
    void multiple_load_store(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 1 | Cond | Soffset8 |
     *
     * Cond - 4 bit condition code
     * Soffset8 - 8-bit signed immediate
     * @param instr - the instruction 
     */
    void conditional_branch(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 0 1 1 1 1 1 | Value8 |
     *
     * Value8 - Comment field
     * @param instr - the instruction 
     */
    void software_interrupt(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * | 1 1 1 0 0 | Offset11 |
     *
     * Offset11 - 11 bits
     * @param instr - the instruction 
     */
    void unconditional_branch(uint16_t instr);

    /**
     * Decodes the 16 bit instruction of the format
     * TODO this is a 32 bit instruction currently I don't support them
     * | 1 1 1 1 | H | Offset |
     *
     * H - Low/high offset bit
     * Offset - 11 bit long branch and link offset high/low
     *
     * @param instr - the instruction 
     */
    void long_branch_with_link(uint32_t instr);

    /**
     * Executes the nop instruction (just skips it);
     * | 0 1 0 0 0 1 1 0 1 1 0 0 0 0 0 0 |
     * @param instr - the instruction
     */
    void nop ( uint16_t instr );
    
    /**
     * Data Synchronization Barrier or Data Memory Barrier - this thing is a 32 bit instruction
     * | 1 1 1 1 0 0 1 1 1 0 1 1 1 1 1 1 1 0 0 0 1 1 1 1 0 1 0 F 1 1 1 1 |
     * TODO 32 bit instruction
     * F is 1 it means Data Memory Barrier (DMB)
     * F is 0 it means Data Synchronization Barrier (DSB)
     * @param instr - the instruction
     */
    void data_mem_sync_barier(uint32_t instr);
    
    /**
     * Instruction Synchronization Barrier - this is a 32 bit instruction
     * | 1 1 1 1 0 0 1 1 1 0 1 1 1 1 1 1 1 0 0 0 1 1 1 1 0 1 1 0 1 1 1 1 |
     * 
     * @param instr - the instruction
     */
    void instruction_sync_barier(uint32_t instr);
    
    /**
     * Change Processor State, Disable or Enable  Interrupts
     * | 1 0 1 1 0 1 1 0 0 1 1 E 0 0 1 0|
     * E - is 0 means enabled (CPSIE)
     * E - is 1 means disabled (CPSID)
     * @param instr - the instruction
     */
    void cpsi_d_e(uint16_t instr);
    
    /**
     * Supervisor Call
     * |1 1 0 1 1 1 1 1| Comment8|
     * TODO currently not implemented
     * Comment8 - is the 8 bit comment field that the handler for the supervisor uses to determine what to do! 
     * @param instr - the instruction (SVC)
     */
    void supervisor_call(uint16_t instr);
    
    /**
     * The breakpoint instruction 
     * TODO currently unimplemented
     *  |1 0 1 1 1 1 1 0| Comment8 |
     * 
     * Comment8 - is the 8 bit comment field so that the breakpoint handler can figure out what to do with this
     * @param instr - the instruction (BKPT)
     */
    void breakpoint(uint16_t instr);
    
    /**
     * | 1 0 1 1 1 1 1 1 0 0 1 F 0 0 0 0 |
     * TODO not implemented
     * F - is 0 means Wait For Event
     * F - is 1 means Wait For Interrupt
     * 
     * @param instr - the instruction
     */
    void wait_for_interupt_event(uint16_t instr);
    
    /**
     * Send Event instruction
     * TODO not implemented
     * | 1 0 1 1 1 1 1 1 0 1 0 0 0 0 0 0 |
     * 
     * @param instr - the instruction
     */
    void send_event(uint16_t instr) ;
    
    /**
     * Sign extend or zero extend a byte or half-word
     */
    void sign_zero_extend_byte_halfword(uint32_t instr);
    
public:

    /**
     * Creates an instance of the cpu
     * @param flash_size the size of the flash size in bytes
     * @param sram_size the sram size in bytes
     */
    cpu(uint32_t flash_size, uint32_t sram_size);


    /**
     * Pushes the register
     * @param instr - the instruction 
     * @param address the address 
     * @param val - the mask of the register
     * @param reg - the register we want to to push
     */
    void push_reg(uint16_t instr, uint32_t &address, int val, int reg);

    /**
     * Pop the register
     * @param instr - the instruction 
     * @param address - the address
     * @param val - the mask of the register
     * @param reg - the register we want to push
     */
    void pop_reg(uint16_t instr, uint32_t &address, int val, int reg);

    void thumb_stm_reg(uint32_t instr, uint32_t &address, int val, int r);

    void thumb_ldm_reg(uint32_t opcode, uint32_t &address, int val, int r);
};


#endif //EMULATOR_M0_DECODER_H
