//
// Created by Dimitrije on 10/28/17.
//

#include <stdexcept>
#include <iostream>
#include "../pheripherals/peripheral.h"
#include "mmu.h"


mmu::mmu(uint8_t *code_region, uint8_t *sram_region) : code_region(code_region), sram_region(sram_region) {}

void mmu::register_peripheral(peripheral *p) {

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

uint32_t mmu::read32(uint32_t address) {
    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        return*((uint32_t*)(&code_region[address - CODE_BEGIN]));
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        return *((uint32_t*)(&sram_region[address - SRAM_BEGIN]));
    }

    return 0;
}

void mmu::write32(uint32_t address, uint32_t value) {
    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        *((uint32_t*)(&code_region[address - CODE_BEGIN])) = value;
        return;
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        *((uint32_t*)(&sram_region[address - SRAM_BEGIN])) = value;
        return;
    }
}

void mmu::write16(uint32_t address, uint16_t value) {
    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        *((uint16_t*)(&code_region[address - CODE_BEGIN])) = value;
        return;
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
         *((uint16_t*)(&sram_region[address - SRAM_BEGIN])) = value;
        return;
    }
}

uint16_t mmu::read16(uint32_t address) {
    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        return*((uint16_t*)(&code_region[address - CODE_BEGIN]));
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        return *((uint16_t*)(&sram_region[address - SRAM_BEGIN]));
    }

    return 0;
}

void mmu::write8(uint32_t address, uint8_t value) {

    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        code_region[address - CODE_BEGIN] = value;
        return;
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        sram_region[address - SRAM_BEGIN] = value;
        return;
    }

    // fix the peripherals
}
uint32_t mmu::read8(uint32_t address) {

    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        return code_region[address - CODE_BEGIN];
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        return sram_region[address - SRAM_BEGIN];
    }

    // this
    return 0;
}

uint16_t mmu::read16s(uint32_t address) {
    // the check if we are writing to code
    if(address <= CODE_END && address >= CODE_BEGIN) {
        return*((uint16_t*)(&code_region[address - CODE_BEGIN]));
    }

    // check if we are writing to sram
    if(address <= SRAM_END && address >= SRAM_BEGIN) {
        return *((uint16_t*)(&sram_region[address - SRAM_BEGIN]));
    }

    return 0;
}

