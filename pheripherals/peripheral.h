//
// Created by dimitrije on 9/11/17.
//

#ifndef EMULATOR_M0_PERIPHERAL_H
#define EMULATOR_M0_PERIPHERAL_H


#include <cstdint>

class peripheral {

protected:

    /**
     * the address where the peripheral starts, must be between 0x3FFFFFFF and 0x60000000
     */
    uint32_t start_address;

    /**
     * the address where the peripheral ends, must be between 0x3FFFFFFF and 0x60000000
     * and greater than the start_address
     */
    uint32_t end_address;

public:

    peripheral(uint32_t start_address, uint32_t end_address) : start_address(start_address), end_address(end_address) {}

    /**
     * writes a byte to the peripheral
     * @param address
     */
    virtual void writeWord(uint32_t address, uint8_t value) = 0;

    /**
     * writes a half-word to the peripheral
     * @param address
     */
    virtual void write(uint32_t address, uint16_t value) = 0;

    /**
     * writes a word to the peripheral
     * @param address
     */
    virtual void write(uint32_t address, uint32_t value) = 0;

    /**
     * reads a byte from the peripheral
     * @param address
     */
    virtual void read(uint32_t address, uint8_t &value) = 0;

    /**
     * reads a half-word from the peripheral
     * @param address
     */
    virtual void read(uint32_t address, uint16_t &value) = 0;

    /**
     * reads a word from the peripheral
     * @param address
     */
    virtual void read(uint32_t address, uint32_t &value) = 0;

    /**
     * returns the start address of the peripheral
     * @return the start address
     */
    inline uint32_t get_start_address() { return start_address; }

    /**
     * returns the end address of the peripheral
     * @return the end address
     */
    inline uint32_t get_end_address() { return end_address; }
};


#endif //EMULATOR_M0_PERIPHERAL_H
