//
// Created by Dimitrije on 10/28/17.
//

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