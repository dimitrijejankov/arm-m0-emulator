//
// Created by dimitrije on 9/11/17.
//

#include <exception>
#include <stdexcept>
#include <iostream>
#include "cpu.h"
#include "util.h"

void cpu::move_shifted_register(uint16_t instr) {

    // the instruction is of format | 0 0 0 | Op | Offset5 | Rs | Rd |
    int rd = instr & REGISTER_MASK;
    int rs = (instr >> 3) & REGISTER_MASK;
    int offset5 = (instr >> 6) & OFFSET_5_MASK;
    int op = (instr >> 11) & OPERATION_2_MASK;

    switch (op) {

        // the case of left shift
        // LSL Rd, Rs, #Offset5
        case 0b00 : {
            uint32_t value;
            psr_register.c = ((registers[rs].to_uint >> (32 - offset5)) & 1) != 0;
            value = registers[rs].to_uint << (uint) offset5;
            registers[rd].to_uint = value;
            psr_register.n = (value & 0x80000000) != 0;
            psr_register.z = value == 0;

            break;
        }
            // the case of logical right shift
            // LSR Rd, Rs, #Offset5
        case 0b01 : {

            uint32_t value;
            psr_register.c = ((registers[rs].to_uint >> (offset5 - 1)) & 1) != 0;
            value = registers[rs].to_uint >> (uint) offset5;
            registers[rd].to_uint = value;
            psr_register.n = (value & 0x80000000) != 0;
            psr_register.z = value == 0;

            break;
        }
            // the case of arithmetic right shift
            // ASR Rd, Rs, #Offset5
        case 0b10 : {

            int32_t value;
            psr_register.c = (((int32_t) registers[rs].to_uint >> (offset5 - 1)) & 1) != 0;
            value = (int32_t) registers[rs].to_uint >> offset5;
            registers[rd].to_uint = (uint) value;
            psr_register.n = (value & 0x80000000) != 0;
            psr_register.z = value == 0;

            break;
        }
        default:
            std::runtime_error("The operation in the move shifted register is unsupported!");
    }
}

void cpu::add_subtract(uint16_t instr) {

    // ADD Rd, Rs, Rn
    // ADD Rd, Rs, #Offset3
    // SUB Rd, Rs, Rn
    // SUB Rd, Rs, #Offset3

    int rd = instr & REGISTER_MASK;
    int rs = (instr >> 3) & REGISTER_MASK;
    uint32_t rn_offset3 = (instr >> 6) & REGISTER_MASK;
    int op = (instr >> 9) & FLAG_MASK;
    int i = (instr >> 10) & FLAG_MASK;

    // figure out what the value we actually want to have
    uint32_t value = (i == 0) ? registers[rn_offset3].to_uint : rn_offset3;

    // do the operation 0 is ADD, 1 is SUB
    registers[rd].to_uint = op == 0 ? registers[rs].to_uint + value : registers[rs].to_uint - value;

    // update the flags
    psr_register.z = registers[rd].to_uint == 0;
    psr_register.n = neg(registers[rd].to_uint) != 0;
    psr_register.c = add_carry(registers[rs].to_uint, value, registers[rd].to_uint);
    psr_register.v = add_overflow(registers[rs].to_uint, value, registers[rd].to_uint);
}

void cpu::move_compare_add_subtract_immediate(uint16_t instr) {

    // | 0 0 1 | Op | Rd | Offset8 |
    uint32_t offset8 = instr & OFFSET_8_MASK;
    int rd = (instr >> 8) & REGISTER_MASK;
    int op = (instr >> 11) & OPERATION_2_MASK;

    switch (op) {

        // move 8-bit immediate value into Rd.
        // MOV Rd, #Offset8
        case 0b00 : {
            // grab the 8 bit value
            registers[rd].to_uint = offset8;

            // update the flags
            psr_register.n = false;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // compare contents of Rd with 8-bit immediate value.
            // CMP Rd, #Offset8
        case 0b01 : {

            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = (instr & 255u);
            uint32_t res = lhs - rhs;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
            // add 8-bit immediate value to contents of Rd and place the result in Rd.
            // ADD Rd, #Offset8
        case 0b10: {

            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = (instr & 255u);
            uint32_t res = lhs + rhs;
            registers[rd].to_uint = res;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = add_carry(lhs, rhs, res);
            psr_register.v = add_overflow(lhs, rhs, res);

            break;
        }
            // subtract 8-bit immediate value from contents of Rd and place the result in Rd.
            // SUB Rd, #Offset8
        case 0b11: {

            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = (instr & 255u);
            uint32_t res = lhs - rhs;
            registers[rd].to_uint = res;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = add_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
        default:
            std::runtime_error("The operation in the move compare add subtract immediate is unsupported!");
    }
}

void cpu::alu_operations(uint16_t instr) {
    // | 0 1 0 0 0 0 | Op | Rs | Rd |
    int rd = instr & REGISTER_MASK;
    int rs = (instr >> 3) & REGISTER_MASK;
    int op = (instr >> 6) & OPERATION_4_MASK;

    switch (op) {
        // AND Rd, Rs
        case 0b0000 : {

            registers[rd].to_uint &= registers[rs].to_uint;

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // EOR Rd, Rs
        case 0b0001 : {

            registers[rd].to_uint ^= registers[rs].to_uint;

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // LSL Rd, Rs
        case 0b0010 : {

            uint32_t value = registers[rs].to_bytes.B0;
            if (value) {
                if (value == 32) {
                    value = 0;
                    psr_register.c = (registers[rd].to_uint & 1) != 0;
                } else if (value < 32) {
                    psr_register.c = ((registers[rd].to_uint >> (32 - value)) & 1) != 0;
                    value = registers[rd].to_uint << value;
                } else {
                    value = 0;
                    psr_register.c = false;
                }
                registers[rd].to_uint = value;
            }

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // LSR Rd, Rs
        case 0b0011 : {

            uint32_t value = registers[rs].to_bytes.B0;
            if (value) {
                if (value == 32) {
                    value = 0;
                    psr_register.c = (registers[rd].to_uint & 0x80000000) != 0;
                } else if (value < 32) {
                    psr_register.c = ((registers[rd].to_uint >> (value - 1)) & 1) != 0;
                    value = registers[rd].to_uint >> value;
                } else {
                    value = 0;
                    psr_register.c = false;
                }
                registers[rd].to_uint = value;
            }

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // ASR Rd, Rs
        case 0b0100 : {

            int32_t value = registers[rs].to_bytes.B0;
            if (value) {
                if (value < 32) {
                    psr_register.c = (((int32_t) registers[rd].to_uint >> (int) (value - 1)) & 1) != 0;
                    value = (int32_t) registers[rd].to_uint >> (int) value;
                    registers[rd].to_uint = (uint32_t) value;
                } else {
                    if (registers[rd].to_uint & 0x80000000) {
                        registers[rd].to_uint = 0xFFFFFFFF;
                        psr_register.c = true;
                    } else {
                        registers[rd].to_uint = 0x00000000;
                        psr_register.c = false;
                    }
                }
            }

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // ADC Rd, Rs
        case 0b0101 : {

            uint32_t value = registers[rs].to_uint;
            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs + rhs + (uint32_t) psr_register.c;
            registers[rd].to_uint = res;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = add_carry(lhs, rhs, res);
            psr_register.v = add_overflow(lhs, rhs, res);

            break;
        }
            // SBC Rd, Rs
        case 0b0110 : {

            uint32_t value = registers[rs].to_uint;
            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs - rhs - !((uint32_t) psr_register.c);
            registers[rd].to_uint = res;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
            // ROR Rd, Rs
        case 0b0111 : {

            uint32_t value = registers[rs].to_bytes.B0;

            if (value) {
                value = value & 0x1f;
                if (value == 0) {
                    psr_register.c = (registers[rd].to_uint & 0x80000000) != 0;
                } else {
                    psr_register.c = ((registers[rd].to_uint >> (value - 1)) & 1) != 0;
                    value = ((registers[rd].to_uint << (32 - value)) |
                             (registers[rd].to_uint >> value));
                    registers[rd].to_uint = value;
                }
            }

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // ROR Rd, Rs
        case 0b1000 : {

            uint32_t value = registers[rs].to_bytes.B0;

            if (value) {
                value = value & 0x1f;
                if (value == 0) {
                    psr_register.c = (registers[rd].to_uint & 0x80000000) != 0;
                } else {
                    psr_register.c = ((registers[rd].to_uint >> (value - 1)) & 1) != 0;
                    value = ((registers[rd].to_uint << (32 - value)) |
                             (registers[rd].to_uint >> value));
                    registers[rd].to_uint = value;
                }
            }

            // update the flags
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;
            psr_register.z = registers[rd].to_uint == 0;

            break;
        }
            // TST Rd, Rs
        case 0b1001 : {

            uint32_t value = registers[rd].to_uint & registers[rs].to_uint;
            psr_register.n = (value & 0x80000000) != 0;
            psr_register.z = value == 0;

            break;
        }
            // NEG Rd, Rs
        case 0b1010 : {

            uint32_t lhs = registers[rs].to_uint;
            uint32_t rhs = 0;
            uint32_t res = rhs - lhs;
            registers[rd].to_uint = res;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(rhs, lhs, res);
            psr_register.v = sub_overflow(rhs, lhs, res);

            break;
        };
            // CMN Rd, Rs
        case 0b1011 : {

            uint32_t value = registers[rs].to_uint;
            uint32_t lhs = registers[rd].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs + rhs;

            // update the flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = add_carry(lhs, rhs, res);
            psr_register.v = add_overflow(lhs, rhs, res);

            break;
        };
            // ORR Rd, Rs
        case 0b1100 : {

            registers[rd].to_uint |= registers[rs].to_uint;

            // update the flags
            psr_register.z = registers[rd].to_uint == 0;
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;

            break;
        };
            // MULS Rd, Rs
        case 0b1101 : {

            uint32_t rm = registers[rd].to_uint;
            registers[rd].to_uint = registers[rs].to_uint * rm;

            psr_register.z = registers[rd].to_uint == 0;
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;

            break;
        };
            // BIC Rd, Rs
        case 0b1110 : {

            registers[rd].to_uint &= (~registers[rs].to_uint);

            // update the flags
            psr_register.z = registers[rd].to_uint == 0;
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;

            break;
        };
            // MVN Rd, Rs
        case 0b1111 : {

            registers[rd].to_uint = ~registers[rs].to_uint;

            // update the flags
            psr_register.z = registers[rd].to_uint == 0;
            psr_register.n = (registers[rd].to_uint & 0x80000000) != 0;

            break;
        };
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }

}

void cpu::hi_register_operations_branch_exchange(uint16_t instr) {

    // | 0 1 0 0 0 1 | Op | H1 | H2 | Rs/Hs | Rd/Hd |
    int op_h1_h2 = (instr >> 6) & 0b1111;

    switch (op_h1_h2) {

        // ADD Rd, Hs
        case 0b0001: {
            registers[instr & 7].to_uint += registers[((instr >> 3) & 7) + 8].to_uint;
            break;
        }
            // ADD Hd, Rs
        case 0b0010: {

            registers[(instr & 7) + 8].to_uint += registers[(instr >> 3) & 7].to_uint;

            // is this the PC register
            if ((instr & 7) == 7) {
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
            // ADD Hd, Hs
        case 0b0011: {
            registers[(instr & 7) + 8].to_uint += registers[((instr >> 3) & 7) + 8].to_uint;

            // is this the PC register
            if ((instr & 7) == 7) {
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
            // CMP Rd, Hs
        case 0b0101: {
            int dest = instr & 7;
            uint32_t value = registers[((instr >> 3) & 7) + 8].to_uint;

            uint32_t lhs = registers[dest].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs - rhs;
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
            // CMP Hd, Rs
        case 0b0110: {
            int dest = (instr & 7) + 8;
            uint32_t value = registers[(instr >> 3) & 7].to_uint;

            uint32_t lhs = registers[dest].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs - rhs;
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
            // CMP Hd, Hs
        case 0b0111: {
            int dest = (instr & 7) + 8;
            uint32_t value = registers[((instr >> 3) & 7) + 8].to_uint;

            uint32_t lhs = registers[dest].to_uint;
            uint32_t rhs = value;
            uint32_t res = lhs - rhs;
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
            // MOV Rd, Hs
        case 0b1001: {
            registers[instr & 7].to_uint = registers[((instr >> 3) & 7) + 8].to_uint;
            break;
        }
            // MOV Hd, Rs
        case 0b1010: {
            registers[(instr & 7) + 8].to_uint = registers[(instr >> 3) & 7].to_uint;

            // is this the PC register
            if ((instr & 7) == 7) {
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
            // MOV Hd, Hs
        case 0b1011: {
            registers[(instr & 7) + 8].to_uint = registers[((instr >> 3) & 7) + 8].to_uint;

            // is this the PC register
            if ((instr & 7) == 7) {
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
            // BX Rs
        case 0b1100:
            // BX Hs
        case 0b1101: {

            int base = (instr >> 3) & 15;
            registers[15].to_uint = registers[base].to_uint;

            if ((registers[15].to_uint & 1) != 0u) {
                // we are in thumb state because the address had a 1 bit set
                psr_register.t = true;

                // remove the last bit of the address since it's not a valid one
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            } else {
                throw std::runtime_error("Going to ARM state is not possible on a M0 cpu");
            }
        }
            // BLX Rs this is used to
        case 0b1110:
            // BLX Rs this is used to
        case 0b1111: {
            int base = (instr >> 3) & 15;
            registers[14].to_uint = registers[15].to_uint;
            registers[15].to_uint = registers[base].to_uint;

            if ((registers[15].to_uint & 1) != 0u) {
                // we are in thumb state because the address had a 1 bit set
                psr_register.t = true;

                // remove the last bit of the address since it's not a valid one
                registers[15].to_uint &= 0xFFFFFFFE;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            } else {
                throw std::runtime_error("Going to ARM state is not possible on a M0 cpu");
            }
        }
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }

}

void cpu::pc_relative_load(uint16_t instr) {
    // LDR Rd, [PC, #Imm] same as ADR Rd, label
    uint32_t address = (registers[15].to_uint & 0xFFFFFFFC) + ((instr & 0xFF) << 2);
    registers[(instr >> 8) & 7].to_uint = mmu_ptr->read32(address);
}

void cpu::load_store_with_register_offset(uint16_t instr) {
    int flags = (instr >> 10) & FLAG_MASK_2;

    switch (flags) {
        // STR Rd, [Rb, Ro]
        case 0b00 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            mmu_ptr->write32(address, registers[instr & 7].to_uint);
            break;
        }
            // STRB Rd, [Rb, Ro]
        case 0b01 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            mmu_ptr->write8(address, registers[instr & 7].to_bytes.B0);
            break;
        };
            // LDR Rd, [Rb, Ro]
        case 0b10 : {

            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = mmu_ptr->read32(address);
            break;
        };
            // LDRB Rd, [Rb, Ro]
        case 0b11 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = mmu_ptr->read8(address);
            break;
        };
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }
}

void cpu::load_store_sign_extended_byte_halfword(uint16_t instr) {

    int flags = (instr >> 10) & FLAG_MASK_2;

    switch (flags) {
        // STRH Rd, [Rb, Ro]
        case 0b00 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            mmu_ptr->write16(address, registers[instr & 7].to_half_words.W0);
            break;
        }
            // LDRH Rd, [Rb, Ro]
        case 0b01 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = mmu_ptr->read16(address);
            break;
        };
            // LDSB Rd, [Rb, Ro]
        case 0b10 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = (int8_t) mmu_ptr->read8(address);
            break;
        };
            // LDSH Rd, [Rb, Ro]
        case 0b11 : {
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = (int16_t) mmu_ptr->read16s(address);
            break;
        };
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }

}

void cpu::load_store_with_immediate_offset(uint16_t instr) {

    int flags = (instr >> 11) & FLAG_MASK_2;

    switch (flags) {

        case 0b00 : {
            // STR Rd, [Rb, #Imm]
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            mmu_ptr->write32(address, registers[instr & 7].to_uint);
            break;
        }
        case 0b10 : {
            // LDR Rd, [Rb, #Imm]
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            registers[instr & 7].to_uint = mmu_ptr->read32(address);
            break;
        }
        case 0b01 : {
            // STRB Rd, [Rb, #Imm]
            uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
            mmu_ptr->write8(address, registers[instr & 7].to_bytes.B0);
            break;
        }
        case 0b11 : {
            // LDRB Rd, [Rb, #Imm]
            uint32_t address = registers[(instr >> 3) & 7].to_uint + (((instr >> 6) & 31));
            registers[instr & 7].to_uint = mmu_ptr->read8(address);
            break;
        }
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }

}

void cpu::load_store_halfword_immediate_offset(uint16_t instr) {

    int flag = (instr >> 11) & 0b1; // grab the bit that is relevant

    if (flag != 0) {
        // STRH Rd, [Rs, Rn]
        uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
        mmu_ptr->write16(address, registers[instr & 7].to_half_words.W0);
    } else {
        // LDRH Rd, [Rb, #Imm]
        uint32_t address = registers[(instr >> 3) & 7].to_uint + registers[(instr >> 6) & 7].to_uint;
        mmu_ptr->write8(address, registers[instr & 7].to_bytes.B0);
    }
}

void cpu::sp_relative_load_store(uint16_t instr) {
    int flag = (instr >> 11) & 0b1; // grab the bit that is relevant

    if (flag != 0) {
        // STR Rd, [SP, #Imm]
        uint32_t address = registers[(instr >> 3) & 7].to_uint + (((instr >> 6) & 31) << 2);
        mmu_ptr->write32(address, registers[instr & 7].to_uint);
    } else {
        // LDR Rd, [SP, #Imm]
        uint32_t address = registers[(instr >> 3) & 7].to_uint + (((instr >> 6) & 31) << 2);
        registers[instr & 7].to_uint = mmu_ptr->read32(address);
    }
}

void cpu::load_address(uint16_t instr) {
    int flag = (instr >> 11) & 0b1; // grab the bit that is relevant

    if (flag != 0) {
        // ADD Rd, PC, #Imm
        registers[(instr >> 8) & 7].to_uint = (registers[15].to_uint & 0xFFFFFFFC) + ((instr & 255) << 2);
    } else {
        // ADD Rd, SP, #Imm
        registers[(instr >> 8) & 7].to_uint = registers[13].to_uint + ((instr & 255) << 2);
    }
}

void cpu::add_offset_to_stack_pointer(uint16_t instr) {
    int flag = (instr >> 7) & 0b1; // grab the bit that is relevant
    int offset = (instr & 127) << 2;

    if (flag != 0) {
        // ADD SP, #Imm
        registers[13].to_uint += offset;
    } else {
        // ADD SP, #-Imm
        registers[13].to_uint -= offset;
    }

}

void cpu::push_pop_registers(uint16_t instr) {

    int flag = ((instr >> 8) & 0b1) | ((instr >> 1) & 0b10);

    switch (flag) {

        case 0b00 : {
            // PUSH { Rlist }

            // this calcuates the new stack pointer (previous and -4 * number of selected registers)
            uint32_t temp = registers[13].to_uint - 4 * cpu_bits_set[instr & 0xff];
            uint32_t address = temp & 0xFFFFFFFC;

            // push the selected registers from R0-R7
            push_reg(instr, address, 1, 0);
            push_reg(instr, address, 2, 1);
            push_reg(instr, address, 4, 2);
            push_reg(instr, address, 8, 3);
            push_reg(instr, address, 16, 4);
            push_reg(instr, address, 32, 5);
            push_reg(instr, address, 64, 6);
            push_reg(instr, address, 128, 7);

            // set the new stack pointer
            registers[13].to_uint = temp;
            break;
        }
        case 0b01 : {
            // PUSH { Rlist, LR }

            // this calculates the new stack pointer (previous - 4 for LR and - 4 * number of selected registers)
            uint32_t temp = registers[13].to_uint - 4 - 4 * cpu_bits_set[instr & 0xff];
            uint32_t address = temp & 0xFFFFFFFC;

            // push the registers selected registers from R0-R7 including the link register
            push_reg(instr, address, 1, 0);
            push_reg(instr, address, 2, 1);
            push_reg(instr, address, 4, 2);
            push_reg(instr, address, 8, 3);
            push_reg(instr, address, 16, 4);
            push_reg(instr, address, 32, 5);
            push_reg(instr, address, 64, 6);
            push_reg(instr, address, 128, 7);
            push_reg(instr, address, 256, 14);

            // set the new stack pointer
            registers[13].to_uint = temp;
            break;
        }
        case 0b10 : {
            // POP { Rlist }

            // remove the last two bits
            uint32_t address = registers[13].to_uint & 0xFFFFFFFC;

            // this calculates the new stack pointer (add 4 * the number of selected registers)
            uint32_t temp = registers[13].to_uint + 4 * cpu_bits_set[instr & 0xFF];

            // pop each selected register (R0-R7) from the stack
            pop_reg(instr, address, 1, 0);
            pop_reg(instr, address, 2, 1);
            pop_reg(instr, address, 4, 2);
            pop_reg(instr, address, 8, 3);
            pop_reg(instr, address, 16, 4);
            pop_reg(instr, address, 32, 5);
            pop_reg(instr, address, 64, 6);
            pop_reg(instr, address, 128, 7);

            // sets the new stack pointer
            registers[13].to_uint = temp;
            break;
        }
        case 0b11 : {
            // POP { Rlist, PC }

            // remove the last two bits
            uint32_t address = registers[13].to_uint & 0xFFFFFFFC;

            // this calculates the new stack pointer (add 4 for the programming counter and
            // add 4 * the number of selected registers)
            uint32_t temp = registers[13].to_uint + 4 + 4 * cpu_bits_set[instr & 0xFF];

            pop_reg(instr, address, 1, 0);
            pop_reg(instr, address, 2, 1);
            pop_reg(instr, address, 4, 2);
            pop_reg(instr, address, 8, 3);
            pop_reg(instr, address, 16, 4);
            pop_reg(instr, address, 32, 5);
            pop_reg(instr, address, 64, 6);
            pop_reg(instr, address, 128, 7);

            // read the program counter from this
            registers[15].to_uint = (mmu_ptr->read32(address) & 0xFFFFFFFE);

            // set the next pc to be the read value
            next_pc = registers[15].to_uint;

            // increase the register to go to the next instruction
            registers[15].to_uint += 2;

            // the stack pointer now becomes the new calculated value
            registers[13].to_uint = temp;

            // prfetch the instruction
            prefetch();

            break;
        }
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }
}

void cpu::multiple_load_store(uint16_t instr) {

    // extract the relevant flag
    int flag = (instr >> 11) & 0b1;

    if (flag != 0) {
        // STMIA Rb!, { Rlist }

        // grab the Rb register
        int reg = (instr >> 8) & 7;

        // figure out the address
        uint32_t address = registers[reg].to_uint & 0xFFFFFFFC;
        uint32_t temp = registers[reg].to_uint + 4 * cpu_bits_set[instr & 0xff];

        // store the selected registers
        thumb_stm_reg(instr, address, 1, 0);
        thumb_stm_reg(instr, address, 2, 1);
        thumb_stm_reg(instr, address, 4, 2);
        thumb_stm_reg(instr, address, 8, 3);
        thumb_stm_reg(instr, address, 16, 4);
        thumb_stm_reg(instr, address, 32, 5);
        thumb_stm_reg(instr, address, 64, 6);
        thumb_stm_reg(instr, address, 128, 7);

        // write back the base address
        registers[reg].to_uint = temp;
    } else {
        // LDMIA Rb!, { Rlist }
        int reg = (instr >> 8) & 7;
        uint32_t address = registers[reg].to_uint & 0xFFFFFFFC;

        // load the selected registers
        thumb_ldm_reg(instr, address, 1, 0);
        thumb_ldm_reg(instr, address, 2, 1);
        thumb_ldm_reg(instr, address, 4, 2);
        thumb_ldm_reg(instr, address, 8, 3);
        thumb_ldm_reg(instr, address, 16, 4);
        thumb_ldm_reg(instr, address, 32, 5);
        thumb_ldm_reg(instr, address, 64, 6);
        thumb_ldm_reg(instr, address, 128, 7);

        // if the register was not written to we write back the base address
        if (!(instr & (1 << reg))) {
            registers[reg].to_uint = address;
        }
    }
}

void cpu::conditional_branch(uint16_t instr) {

    int flag = (instr >> 8) & FLAG_MASK_4;
    auto offset = (int8_t) (instr & 0xFF);

    switch (flag) {

        case 0000 : {
            // BEQ label
            if (psr_register.z) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0001 : {
            // BNE label
            if (!psr_register.z) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0010 : {
            // BCS label
            if (psr_register.c) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0011 : {
            // BCC label
            if (!psr_register.c) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0100 : {
            // BMI label
            if (psr_register.n) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0101 : {
            // BPL label
            if (!psr_register.n) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }

            break;
        }
        case 0110 : {
            // BVS label
            if (psr_register.v) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 0111 : {
            // BVC label
            if (!psr_register.v) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1000 : {
            // BHI label
            if (psr_register.c && !psr_register.z) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1001 : {
            //  BLS label
            if (!psr_register.c || psr_register.z) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1010 : {
            // BGE label
            if (psr_register.n == psr_register.v) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1011 : {
            // BLT label
            if (psr_register.n != psr_register.v) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1100 : {
            // BGT label
            if (!psr_register.z && (psr_register.n == psr_register.v)) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        case 1101 : {
            // BLE label
            if (psr_register.z || (psr_register.n != psr_register.v)) {
                registers[15].to_uint += offset << 1;
                next_pc = registers[15].to_uint;
                registers[15].to_uint += 2;
                prefetch();
            }
            break;
        }
        default:
            std::runtime_error("The operation in the is unsupported!");
    }
}


void cpu::unconditional_branch(uint16_t instr) {

    int offset = (instr & 0x3FF) << 1;
    if (instr & 0x0400) {
        offset |= 0xFFFFF800;
    }

    registers[15].to_uint += offset;
    next_pc = registers[15].to_uint;
    registers[15].to_uint += 2;

    prefetch();
}

void cpu::long_branch_with_link(uint32_t instr) {

    // grab the offset
    int offset = (instr & 0x7FF);

    // backward or forward
    if ((offset & 0xF000) != 0) {
        registers[14].to_uint = registers[15].to_uint + (offset << 12);
    } else {
        registers[14].to_uint = registers[15].to_uint + ((offset << 12) | 0xFF800000);
    }

    // grab the other part of the instruction
    instr = instr >> 16;

    offset = (instr & 0x7FF);
    uint32_t temp = registers[15].to_uint - 2;
    registers[15].to_uint = (registers[14].to_uint + (offset << 1)) & 0xFFFFFFFE;
    next_pc = registers[15].to_uint;
    registers[15].to_uint += 2;
    registers[14].to_uint = temp | 1;

    prefetch();
}

void cpu::nop(uint16_t instr) {}


void cpu::software_interrupt(uint16_t instr) {
    std::runtime_error("Software interrupt is not implemented");
}

void cpu::data_mem_sync_barier(uint32_t instr) {
    // TODO figure out what this does!
    std::runtime_error("Data Memory Barrier and Data Synchronization Barrier not implemented yet!");
}

void cpu::cpsi_d_e(uint16_t instr) {
    std::runtime_error("CPSIE and CPSID are not implemented yet!");
}

void cpu::supervisor_call(uint16_t instr) {
    std::runtime_error("The supervisor call instruction is not implemented yet!");
}

void cpu::breakpoint(uint16_t instr) {
    std::runtime_error("The breakpoint instruction is not implemented yet!");
}

void cpu::wait_for_interupt_event(uint16_t instr) {
    std::runtime_error("Wait For Event and Wait For Interrupt are not implemented yet!");
}

void cpu::send_event(uint16_t instr) {
    std::runtime_error("Send Event is not implemented yet!");
}

void cpu::instruction_sync_barier(uint32_t instr) {
    std::runtime_error("Instruction Synchronization Barrier is not implemented yet");
}

void cpu::sign_zero_extend_byte_halfword(uint32_t instr) {
    std::runtime_error("Sign extend or zero extend is not implemented yet");
}

void cpu::execute_op(uint16_t instruction) {

    if (instruction == 0b0100011011000000) {
        nop(instruction);
    } else if ((0b1111111111101111 & instruction) == 0b1011011001100010) {
        cpsi_d_e(instruction);
    } else if ((0b1111111111101111 & instruction) == 0b1011111100100000) {
        wait_for_interupt_event(instruction);
    } else if (instruction == 0b1011111101000000) {
        send_event(instruction);
    } else if ((instruction & 0xFF00) == 0b1101111100000000) {
        supervisor_call(instruction);
    } else if ((instruction & 0xFF00) == 0b1101111000000000) {
        breakpoint(instruction);
    } else if ((instruction & 0xFF00) == 0b1101111100000000) {
        software_interrupt(instruction);
    } else if ((instruction & 0xFF00) == 0b1011000000000000) {
        add_offset_to_stack_pointer(instruction);
    } else if ((instruction & 0b1111001000000000) == 0b0101000000000000) {
        load_store_with_register_offset(instruction);
    } else if ((instruction & 0b1111001000000000) == 0b0101001000000000) {
        load_store_sign_extended_byte_halfword(instruction);
    } else if ((instruction & 0b1111011000000000) == 0b1011010000000000) {
        push_pop_registers(instruction);
    } else if ((instruction & 0b1111110000000000) == 0b0100000000000000) {
        alu_operations(instruction);
    } else if ((instruction & 0b1111110000000000) == 0b0100010000000000) {
        hi_register_operations_branch_exchange(instruction);
    } else if ((instruction & 0b1111100000000000) == 0b0001100000000000) {
        add_subtract(instruction);
    } else if ((instruction & 0b1111100000000000) == 0b0100100000000000) {
        pc_relative_load(instruction);
    } else if ((instruction & 0b1111100000000000) == 0b0111000000000000) {
        unconditional_branch(instruction);
    } else if ((instruction & 0b1111100000000000) == 0b0111000000000000) {
        unconditional_branch(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1000000000000000) {
        load_store_halfword_immediate_offset(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1001000000000000) {
        sp_relative_load_store(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1010000000000000) {
        load_address(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1100000000000000) {
        multiple_load_store(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1101000000000000) {
        conditional_branch(instruction);
    } else if ((instruction & 0b1111000000000000) == 0b1111000000000000) {
        long_branch_with_link(instruction);
    } else if ((instruction & 0b1110000000000000) == 0b0010000000000000) {
        move_compare_add_subtract_immediate(instruction);
    } else if ((instruction & 0b1110000000000000) == 0b0000000000000000) {
        move_shifted_register(instruction);
    } else if ((instruction & 0b1110000000000000) == 0b0110000000000000) {
        load_store_with_immediate_offset(instruction);
    } else {
        std::runtime_error("This instruction is unknown or unimplemented");
    }
}


void cpu::prefetch() {
    cpu_prefetch[0] = mmu_ptr->read16(next_pc);
    cpu_prefetch[1] = mmu_ptr->read16(next_pc + 2);
}

void cpu::prefetch_next() {
    cpu_prefetch[1] = mmu_ptr->read16(next_pc + 2);
}

void cpu::push_reg(uint16_t instr, uint32_t &address, int val, int reg) {
    if (instr & val) {
        mmu_ptr->write32(address, registers[reg].to_uint);
        address += 4;
    }
}

void cpu::pop_reg(uint16_t instr, uint32_t &address, int val, int reg) {
    if (instr & val) {
        registers[reg].to_uint = mmu_ptr->read32(address);
        address += 4;
    }
}

void cpu::thumb_stm_reg(uint32_t instr, uint32_t &address, int val, int r) {
    if (instr & val) {
        mmu_ptr->write32(address, registers[r].to_uint);
        address += 4;
    }
}

void cpu::thumb_ldm_reg(uint32_t instr, uint32_t &address, int val, int r) {
    if (instr & (val)) {
        registers[(r)].to_uint = mmu_ptr->read32(address);
        address += 4;
    }
}

cpu::cpu(uint32_t flash_size, uint32_t sram_size) {

    // init the mmu by allocating the flash region and the sram region
    mmu_ptr = new mmu(new uint8_t[flash_size], new uint8_t[sram_size]);

    // resets the cpu
    reset();

    // initializes the cpu bits set
    init_cpu_bits_set();
}

cpu::cpu(uint8_t *flash, uint8_t *sram) {
    // init the mmu by allocating the flash region and the sram region
    mmu_ptr = new mmu(flash, sram);

    // resets the cpu
    reset();

    // initializes the cpu bits set
    init_cpu_bits_set();
}

void cpu::reset() {

    // the default cpu mode is thread mode
    current_mode = THREAD_MODE;

    // set the psr
    psr_register.t = true;

    // initializes the programming counter
    next_pc = mmu_ptr->read32(PC_INIT_ADDRESS);

    registers[15].to_uint = next_pc + 2;
}

void cpu::run() {

    do {

        uint16_t instr = cpu_prefetch[0];
        cpu_prefetch[0] = cpu_prefetch[1];

        next_pc = registers[15].to_uint;
        registers[15].to_uint += 2;

        // fetch the next instruction
        prefetch_next();

        execute_op(instr);

    } while (!holdState);
}

void cpu::run(size_t n_instr) {

    prefetch();

    do {

        uint16_t instr = cpu_prefetch[0];
        cpu_prefetch[0] = cpu_prefetch[1];

        next_pc = registers[15].to_uint;
        registers[15].to_uint += 2;

        // fetch the next instruction
        prefetch_next();

        execute_op(instr);

    } while (!holdState && --n_instr);
}

void cpu::verbose_run(size_t n_instr) {

    // print out the starting PC
    std::cout << std::hex << "The starting PC : " << next_pc << std::endl;

    // prefetch the starting instructions
    prefetch();

    do {

        uint16_t instr = cpu_prefetch[0];
        cpu_prefetch[0] = cpu_prefetch[1];

        std::cout << "Executing instruction :" << std::hex << instr << std::endl;

        next_pc = registers[15].to_uint;
        registers[15].to_uint += 2;

        // fetch the next instruction
        prefetch_next();

        execute_op(instr);

    } while (!holdState && --n_instr);
}


mmu *cpu::get_mmu() {
    return mmu_ptr;
}

arm_register_t *cpu::get_registers() {
    return registers;
}

psr cpu::get_psr() {
    return psr_register;
}

void cpu::print() {

    std::cout << std::endl << "The state of the CPU" << std::endl;

    // show the registers
    for(int i = 0; i < 15; ++i) {
        std::cout << "The register " + std::to_string(i) + " has value : " << std::hex << registers[i].to_uint << std::endl;
    }

    // split it
    std::cout << std::endl;

    // the psr register values
    std::cout << "T : " << psr_register.t << std::endl;
    std::cout << "C : " << psr_register.c << std::endl;
    std::cout << "N : " << psr_register.n << std::endl;
    std::cout << "V : " << psr_register.v << std::endl;
}