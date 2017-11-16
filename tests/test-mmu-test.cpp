#include <gtest/gtest.h>
#include <cstring>
#include "mmu.h"


class test_mmu: public testing::Test {
public:

    // the instance of the mmu
    mmu *instance;

    // the code and the sram regions
    uint8_t* code_region;
    uint8_t *sram_region;
    
    test_mmu( ) {
        // initialize the region
        code_region = new uint8_t[1024u];
        sram_region = new uint8_t[1024u];

        // create the instance
        instance = new mmu(code_region, sram_region);
    }

    void SetUp() override {
        // code here will execute just before the test ensues
        std::memset(code_region, 0, sizeof(uint8_t) * 1024u);
        std::memset(sram_region, 0, sizeof(uint8_t) * 1024u);
    }

    void TearDown() override {
        // code here will be called just after the test completes
        // ok to through exceptions from here if need be
    }

    ~test_mmu() override {
        // do the cleanup
        delete instance;
        delete[] code_region;
        delete[] sram_region;
    }
};

TEST_F(test_mmu, sram_write_read_8)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 1024u; ++i) {
        this->instance->write8(SRAM_BEGIN + i * sizeof(uint8_t), static_cast<uint8_t>(i % 256));
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 1024u; ++i) {
        EXPECT_EQ(this->instance->read8(SRAM_BEGIN + i * sizeof(uint8_t)), static_cast<uint8_t>(i % 256));
    }
}

TEST_F(test_mmu, sram_write_read_16)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 512u; ++i) {
        this->instance->write16(SRAM_BEGIN + i * sizeof(uint16_t), static_cast<uint16_t>(i % 65536));
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 512u; ++i) {
        EXPECT_EQ(this->instance->read16(SRAM_BEGIN + i * sizeof(uint16_t)), static_cast<uint16_t>(i % 65536));
    }
}

TEST_F(test_mmu, sram_write_read_32)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 256u; ++i) {
        this->instance->write32(SRAM_BEGIN + i * sizeof(uint32_t), i);
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 256u; ++i) {
        EXPECT_EQ(this->instance->read32(SRAM_BEGIN + i * sizeof(uint32_t)), i);
    }
}

TEST_F(test_mmu, code_write_read_8)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 1024u; ++i) {
        this->instance->write8(CODE_BEGIN + i * sizeof(uint8_t), static_cast<uint8_t>(i % 256));
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 1024u; ++i) {
        EXPECT_EQ(this->instance->read8(CODE_BEGIN + i * sizeof(uint8_t)), static_cast<uint8_t>(i % 256));
    }
}

TEST_F(test_mmu, code_write_read_16)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 512u; ++i) {
        this->instance->write16(CODE_BEGIN + i * sizeof(uint16_t), static_cast<uint16_t>(i % 65536));
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 512u; ++i) {
        EXPECT_EQ(this->instance->read16(CODE_BEGIN + i * sizeof(uint16_t)), static_cast<uint16_t>(i % 65536));
    }
}

TEST_F(test_mmu, code_write_read_32)
{
    // write stuff to the sram
    for(uint32_t i = 0; i < 256u; ++i) {
        this->instance->write32(CODE_BEGIN + i * sizeof(uint32_t), i);
    }

    // read stuff from the sram
    for(uint32_t i = 0; i < 256u; ++i) {
        EXPECT_EQ(this->instance->read32(CODE_BEGIN + i * sizeof(uint32_t)), i);
    }
}
