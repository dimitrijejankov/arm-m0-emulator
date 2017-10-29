//
// Created by Dimitrije on 10/28/17.
//

#ifndef EMULATOR_M0_MMU_H
#define EMULATOR_M0_MMU_H


#include <vector>

class mmu {
private:

    /**
    * The memory in on-chip flash ranges from 0x00000000 to 0x1FFFFFFF
    */
    uint8_t *code_region;

    /**
     * The region in on-chip SRAM rages from 0x20000000 to 0x3FFFFFFF
     */
    uint8_t *sram_region;

    /**
     * The list of all peripheral this cpu has
     */
    std::vector<peripheral*> peripherals;

public:

    mmu(uint8_t *code_region, uint8_t *sram_region);

    /**
     * Registers a peripheral to the mmu
     * @param p
     */
    void register_peripheral(peripheral *p);
};


#endif //EMULATOR_M0_MMU_H
