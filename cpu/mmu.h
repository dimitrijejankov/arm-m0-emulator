//
// Created by Dimitrije on 10/28/17.
//

#ifndef EMULATOR_M0_MMU_H
#define EMULATOR_M0_MMU_H

#include <vector>
#include <peripheral.h>

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
  std::vector<peripheral *> peripherals;

public:

  mmu(uint8_t *code_region, uint8_t *sram_region);

  /**
   * Registers a peripheral to the mmu
   * @param p the peripheral we want to add
   */
  void register_peripheral(peripheral *p);

  /**
   * Reads a 32 bit value from a given address
   * @param address 32 bit address
   * @return the value that was read
   */
  uint32_t read32(uint32_t address);

  /**
   * Writes a 32 bit value to an 32 bit address
   * @param address the 32 bit address
   * @param value the value we want to write
   */
  void write32(uint32_t address, uint32_t value);
  void write8(uint32_t address, uint8_t value);
  uint32_t read8(uint32_t address);
  void write16(uint32_t address, uint16_t value);
  uint16_t read16(uint32_t address);
  uint16_t read16s(uint32_t i);
};

#endif //EMULATOR_M0_MMU_H
