//
// Created by dimitrije on 9/11/17.
//

#include "cpu.h"

void cpu::move_shifted_register(uint16_t instr) {

}

void cpu::add_subtract(uint16_t instr) {

}

void cpu::move_compare_add_subtract_immediate(uint16_t instr) {

}

void cpu::alu_operations(uint16_t instr) {

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