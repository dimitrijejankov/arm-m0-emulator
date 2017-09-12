//
// Created by dimitrije on 9/11/17.
//

#include <exception>
#include <stdexcept>
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
        case 0b00 : {
            // shift the source register by the offset
            registers[rd] = registers[rs] << offset5;

            // update the flags
            psr_register.c = ((to_signed(registers[rs]) >> (32 - offset5)) & 1) != 0;
            psr_register.n = (registers[rd] & 0x80000000) != 0u;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // the case of logical right shift
        case 0b01 : {
            // logical shift the source register to the right by the offset
            registers[rd] = registers[rs] >> offset5;

            // update the flags
            psr_register.c = ((to_signed(registers[rs]) >> (32 - offset5)) & 1) != 0;
            psr_register.n = (registers[rd] & 0x80000000) != 0u;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // the case of arithmetic right shift
        case 0b10 : {
            // arithmetic shift the source register to the right by the offset
            int32_t tmp = to_signed(registers[rs]) >> offset5;
            registers[rd] = to_unsigned(tmp);

            // update flags
            psr_register.c = ((to_signed(registers[rs]) >> (offset5 - 1)) & 1) != 0;
            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0u;

            break;
        }
        default:
            std::runtime_error("The operation in the move shifted register is unsupported!");
    }
}

void cpu::add_subtract(uint16_t instr) {

    // | 0 0 0 1 1 | I | Op | Rn/offset3 | Rs | Rd |
    int rd = instr & REGISTER_MASK;
    int rs = (instr >> 3) & REGISTER_MASK;
    uint32_t rn_offset3 = (instr >> 6) & REGISTER_MASK;
    int op = (instr >> 9) & FLAG_MASK;
    int i = (instr >> 10) & FLAG_MASK;

    // figure out what the value we actually want to have
    uint32_t value = (i == 0) ? registers[rs] : rn_offset3;

    // do the operation 0 is ADD, 1 is SUB
    registers[rd] = op == 0 ? registers[rs] + value : registers[rs] - value;

    // update the flags
    psr_register.z = registers[rd] == 0;
    psr_register.n = neg(registers[rd]) != 0;
    psr_register.c = add_carry(registers[rs], value, registers[rd]);
    psr_register.v = add_overflow(registers[rs], value, registers[rd]);
}

void cpu::move_compare_add_subtract_immediate(uint16_t instr) {
    // | 0 0 1 | Op | Rd | Offset8 |
    uint32_t offset8 = instr & OFFSET_8_MASK;
    int rd = (instr >> 8) & REGISTER_MASK;
    int op = (instr >> 10) & REGISTER_MASK;

    switch (op) {

        // move 8-bit immediate value into Rd.
        case 0b00 : {
            // just copy
            registers[rd] = offset8;

            // update flags
            psr_register.n = false;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // compare contents of Rd with 8-bit immediate value.
        case 0b01 : {

            // store the lhs for reuse
            uint32_t tmp = registers[rd];

            // do the operation
            registers[rd] = registers[rd] - offset8;

            // update flags
            psr_register.z = registers[rd] == 0;
            psr_register.n = neg(registers[rd]) != 0;
            psr_register.c = sub_carry(tmp, offset8, registers[rd]);
            psr_register.v = sub_overflow(tmp, offset8, registers[rd]);

            break;
        }
            // add 8-bit immediate value to contents of Rd and place the result in Rd.
        case 0b10: {
            // store the lhs for reuse
            uint32_t tmp = registers[rd];

            // do the operation
            registers[rd] = registers[rd] + offset8;

            // update flags
            psr_register.z = registers[rd] == 0;
            psr_register.n = neg(registers[rd]) != 0;
            psr_register.c = add_carry(tmp, offset8, registers[rd]);
            psr_register.v = add_overflow(tmp, offset8, registers[rd]);

            break;
        }
            // subtract 8-bit immediate value from contents of Rd and place the result in Rd.
        case 0b11: {
            // store the lhs for reuse
            uint32_t tmp = registers[rd];

            // do the operation
            registers[rd] = registers[rd] - offset8;

            // update flags
            psr_register.z = registers[rd] == 0;
            psr_register.n = neg(registers[rd]) != 0;
            psr_register.c = add_carry(tmp, offset8, registers[rd]);
            psr_register.v = sub_overflow(tmp, offset8, registers[rd]);

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
    int op = (instr >> 6) & OPERATION_3_MASK;

    switch (op) {
        // AND Rd, Rs
        case 0b0000 : {

            // perform the and operation
            registers[rd] &= registers[rs];

            // update flags
            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;
            break;
        }
            // EOR Rd, Rs
        case 0b0001 : {

            // perform the and operation
            registers[rd] ^= registers[rs];

            // update flags
            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // LSL Rd, Rs
        case 0b0010 : {

            uint32_t value = registers[rs] & 0b11111111;
            if (value != 0) {
                if (value == 32) {
                    value = 0;
                    psr_register.c = (registers[rd] & 1) != 0;
                } else if (value < 32) {
                    psr_register.c = ((registers[rd] >> (32 - value)) & 1) != 0;
                    value = registers[rd] << value;
                } else {
                    value = 0;
                    psr_register.c = false;
                }
                registers[rd] = value;
            }

            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // LSR Rd, Rs
        case 0b0011 : {
            uint32_t value = registers[rs] & 0b11111111;
            if (value != 0) {
                if (value == 32) {
                    value = 0;
                    psr_register.c = (registers[rd] & 0x80000000) != 0;
                } else if (value < 32) {
                    psr_register.c = ((registers[rd] >> (value - 1)) & 1) != 0;
                    value = registers[rd] >> value;
                } else {
                    value = 0;
                    psr_register.c = false;
                }
                registers[rd] = value;
            }
            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // ASR Rd, Rs
        case 0b0100 : {

            uint32_t value = registers[rs] & 0b11111111;
            if (value != 0) {
                if (value < 32) {
                    psr_register.c = ((to_signed(registers[rd]) >> (int) (value - 1)) & 1) != 0;
                    int32_t tmp = to_signed(registers[rd]) >> (int) value;
                    value = to_unsigned(tmp);
                    registers[rd] = value;
                } else {
                    if ((registers[rd] & 0x80000000) != 0u) {
                        registers[rd] = 0xFFFFFFFF;
                        psr_register.c = true;
                    } else {
                        registers[rd] = 0x00000000;
                        psr_register.c = false;
                    }
                }
            }

            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // ADC Rd, Rs
        case 0b0101 : {

            // fetch the values
            uint32_t value = registers[rs];
            uint32_t lhs = registers[rd];
            uint32_t rhs = value;

            // perform the operation
            uint32_t res = lhs + rhs + (uint32_t) psr_register.c;
            registers[rd] = res;

            // update flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = add_carry(lhs, rhs, res);
            psr_register.v = add_overflow(lhs, rhs, res);

            break;
        }
            // SBC Rd, Rs
        case 0b0110 : {

            // fetch the values
            uint32_t value = registers[rs];
            uint32_t lhs = registers[rd];
            uint32_t rhs = value;

            // perform the operation
            uint32_t res = lhs - rhs - (uint32_t) !(psr_register.c);
            registers[rd] = res;

            // update flags
            psr_register.z = res == 0;
            psr_register.n = neg(res) != 0;
            psr_register.c = sub_carry(lhs, rhs, res);
            psr_register.v = sub_overflow(lhs, rhs, res);

            break;
        }
        case 0b0111 : {

            // fetch the values
            uint32_t value = registers[rs] & 0b11111111;

            if (value != 0)
            {
                value = value & 0x1f;
                if (value == 0)
                {
                    psr_register.c = (registers[rd] & 0x80000000) != 0;
                }
                else
                {
                    psr_register.c = ((registers[rd] >> (value - 1)) & 1) != 0;
                    value = ((registers[rd] << (32 - value)) |
                             (registers[rd] >> value));
                    registers[rd] = value;
                }
            }

            // update flags
            psr_register.n = (registers[rd] & 0x80000000) != 0;
            psr_register.z = registers[rd] == 0;

            break;
        }
            // ROR Rd, Rs
        case 0b1000 :
            break;
            // TST Rd, Rs
        case 0b1001 :
            break;
            // NEG Rd, Rs
        case 0b1010 :
            break;
        case 0b1011 :
            break;
        case 0b1100 :
            break;
        case 0b1101 :
            break;
        case 0b1110 :
            break;
        case 0b1111 :
            break;
        default:
            std::runtime_error("The operation in the alu is unsupported!");
    }

}

void cpu::hi_register_operations_branch_exchange(uint16_t instr) {

}

void cpu::pc_relative_load(uint16_t instr) {

}

void cpu::load_store_with_register_offset(uint16_t instr) {

}

void cpu::load_store_sign_extended_byte_halfword(uint16_t instr) {

}

void cpu::load_store_with_immediate_offset(uint16_t instr) {

}

void cpu::load_store_halfword_immediate_offset(uint16_t instr) {

}

void cpu::sp_relative_load_store(uint16_t instr) {

}

void cpu::load_address(uint16_t instr) {

}

void cpu::add_offset_to_stack_pointer(uint16_t instr) {

}

void cpu::push_pop_registers(uint16_t instr) {

}

void cpu::multiple_load_store(uint16_t instr) {

}

void cpu::conditional_branch(uint16_t instr) {

}

void cpu::software_interrupt(uint16_t instr) {

}

void cpu::unconditional_branch(uint16_t instr) {

}

void cpu::long_branch_with_link(uint16_t instr) {

}

cpu::cpu(uint32_t flash_size, uint32_t sram_size) {

    // resets the cpu
    reset();

    // init the flash region
    this->code_region = new uint8_t[flash_size];

    // init the sram region
    this->sram_region = new uint8_t[sram_size];
}

void cpu::reset() {

    // the default cpu mode is thread mode
    current_mode = THREAD_MODE;
}

void cpu::register_peripheral(peripheral *p) {

    // check if there is a peripheral that is conflicting with this one
    for (auto r : peripherals) {
        if (r->in_conflict(p)) {
            throw std::runtime_error(
                    "could not register the peripheral peripheral : " + p->get_name() + " in conflict with : " +
                    r->get_name() + "\n");
        }
    }

    // add the peripheral
    peripherals.push_back(p);
}
