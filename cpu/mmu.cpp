//
// Created by Dimitrije on 10/28/17.
//

#include <stdexcept>
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
  return 0;
}

void mmu::write32(uint32_t address, uint32_t value) {

}
void mmu::write8(uint32_t address, uint8_t value) {

}
uint32_t mmu::read8(uint32_t address) {
  return 0;
}
void mmu::write16(uint32_t address, uint16_t value) {

}
uint32_t mmu::read16(uint32_t address) {
  return 0;
}

uint16_t mmu::read16s(uint32_t address) {

}
