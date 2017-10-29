//
// Created by dimitrije on 9/11/17.
//

#include <exception>
#include <stdexcept>
#include <zconf.h>
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

    uint32_t value;
    psr_register.c = ((registers[rs].to_uint >> (32 - offset5)) & 1) != 0;
    value = registers[rs].to_uint << (uint) offset5;
    registers[rd].to_uint = value;
    psr_register.n = (value & 0x80000000) != 0;
    psr_register.z = value == 0;

    break;
  }
    // the case of logical right shift
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
  case 0b10 : {

    int32_t value;
    psr_register.c = (((int32_t) registers[rs].to_uint >> (offset5 - 1)) & 1) != 0;
    value = (int32_t) registers[rs].to_uint >> offset5;
    registers[rd].to_uint = (uint) value;
    psr_register.n = (value & 0x80000000) != 0;
    psr_register.z = value == 0;

    break;
  }
  default:std::runtime_error("The operation in the move shifted register is unsupported!");
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
  uint32_t value = (i == 0) ? registers[rs].to_uint : rn_offset3;

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
  int op = (instr >> 10) & REGISTER_MASK;

  switch (op) {

    // move 8-bit immediate value into Rd.
  case 0b00 : {

    // grab the 8 bit value
    registers[rd].to_uint = offset8;

    // update the flags
    psr_register.n = false;
    psr_register.z = registers[rd].to_uint == 0;

    break;
  }
    // compare contents of Rd with 8-bit immediate value.
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
  default:std::runtime_error("The operation in the move compare add subtract immediate is unsupported!");
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
    // MUL Rd, Rs
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
  default:std::runtime_error("The operation in the alu is unsupported!");
  }

}

void cpu::hi_register_operations_branch_exchange(uint16_t instr) {

  // | 0 1 0 0 0 1 | Op | H1 | H2 | Rs/Hs | Rd/Hd |
  int op_h1_h2 = instr >> 6 & 0x1111;

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
  case 0b1100: {

    int base = (instr >> 3) & 15;
    registers[15].to_uint = registers[base].to_uint;

    registers[15].to_uint &= 0xFFFFFFFE;
    next_pc = registers[15].to_uint;
    registers[15].to_uint += 2;
    prefetch();

    break;
  }
    // BX Hs TODO
  case 0b1101: {
    break;
  }
  default:std::runtime_error("The operation in the alu is unsupported!");
  }

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

  // init the mmu by allocating the flash region and the sram region
  mmu_ptr = new mmu(new uint8_t[flash_size], new uint8_t[sram_size]);
}

void cpu::reset() {

  // the default cpu mode is thread mode
  current_mode = THREAD_MODE;
}



void cpu::execute_op(uint16_t) {

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
void cpu::prefetch() {

}
void cpu::prefetch_next() {

}

