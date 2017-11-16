//
// Created by dimitrije on 11/15/17.
//

#include <gtest/gtest.h>
#include "cpu.h"

/**
 * The address where the the code begins
 */
const uint32_t CODE_INIT_ADDRESS = 0x00000058;

/**
 * Sets up the
 */
class test_cpu: public testing::Test {
public:

    // the instance of the mmu
    cpu *instance;

    test_cpu( ) {

        // create the instance
        instance = new cpu(1024u, 1024u);
    }

    void SetUp() override {
        // clear sram
        for(uint32_t i = 0; i < 256u; ++i) {
            this->instance->get_mmu()->write32(SRAM_BEGIN + i * sizeof(uint32_t), 0u);
        }

        // clear code
        for(uint32_t i = 0; i < 256u; ++i) {
            this->instance->get_mmu()->write32(CODE_BEGIN + i * sizeof(uint32_t), 0u);
        }
    }

    void TearDown() override {}

    ~test_cpu() override {
        // do the cleanup
        delete instance;
    }
};

/**
 * This test executes the following instructions :
 *
 * MOV R0, #12
 * MOV R1, #1
 * ADD R0, R1
 *
 * The results should be :
 * R0 = 13
 * R1 = 1
 *
 * All the status registers flags should be false except for the thumb state.
 */
TEST_F(test_cpu, test_cpu_mov_add)
{
    // store the init address and instructions
    instance->get_mmu()->write32(PC_INIT_ADDRESS, CODE_INIT_ADDRESS);

    // MOV R0, #12
    instance->get_mmu()->write16(CODE_INIT_ADDRESS, 0x200C);

    // MOV R1, #1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 2, 0x2101);

    // ADD R0, R1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 4, 0x1840);

    // reset the cpu
    instance->reset();

    // run for three instructions
    instance->run(3);

    // grab the registers from the cpu
    arm_register_t* regs = instance->get_registers();

    // check the results
    EXPECT_EQ(regs[0].to_uint, 13);
    EXPECT_EQ(regs[1].to_uint, 1);

    // grab the program status register
    psr psr_register= instance->get_psr();

    // check it's state
    EXPECT_EQ(psr_register.n, false);
    EXPECT_EQ(psr_register.z, false);
    EXPECT_EQ(psr_register.c, false);
    EXPECT_EQ(psr_register.v, false);
    EXPECT_EQ(psr_register.t, true);
}

/**
 * This test executes the following instructions :
 *
 * MOV R0, #12
 * MOV R1, #13
 * SUB R0, R1
 *
 * The results should be :
 * R0 = 13
 * R1 = 1
 *
 * All the status registers flags should be false except for the thumb state and negative.
 */
TEST_F(test_cpu, test_cpu_mov_sub)
{
    // store the init address and instructions
    instance->get_mmu()->write32(PC_INIT_ADDRESS, CODE_INIT_ADDRESS);

    // MOV R0, #12
    instance->get_mmu()->write16(CODE_INIT_ADDRESS, 0x200C);

    // MOV R1, #1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 2, 0x210D);

    //ADD R0, R1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 4, 0x1A40);

    // reset the cpu
    instance->reset();

    // run for three instructions
    instance->run(3);

    // grab the registers from the cpu
    arm_register_t* regs = instance->get_registers();

    // check the results
    EXPECT_EQ(regs[0].to_uint, -1);
    EXPECT_EQ(regs[1].to_uint, 13);

    // grab the program status register
    psr psr_register= instance->get_psr();

    // check it's state
    EXPECT_EQ(psr_register.n, true);
    EXPECT_EQ(psr_register.z, false);
    EXPECT_EQ(psr_register.c, false);
    EXPECT_EQ(psr_register.v, false);
    EXPECT_EQ(psr_register.t, true);
}

/**
 * This test executes the following instructions :
 *
 * 1. MOV R0, #12
 * 2. MOV R1, #1
 * 3. MOV R2, R15
 * 4. ADD R2, #1
 * 5. SUB R0, R1
 * 6. BEQ loop
 * 7. BX R2
 * 8. loop:
 *
 * The code loops until the value of R0 is 0. The results should be :
 * R0 = 0
 * R1 = 1
 *
 * All the status registers flags should be false except for the thumb state and zero.
 */
TEST_F(test_cpu, test_cpu_loop)
{
    // store the init address and instructions
    instance->get_mmu()->write32(PC_INIT_ADDRESS, CODE_INIT_ADDRESS);

    // MOV R0, #12
    instance->get_mmu()->write16(CODE_INIT_ADDRESS, 0x200C); // 8204

    // MOV R1, #1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 2, 0x2101); // 8449

    // MOV R2, R15
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 4, 0x467A); // 18042

    // ADD R2, #1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 6, 0x3201); // 12801

    // SUB R0, R1
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 8, 0x1A40); // 6720

    // BEQ loop
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 10, 0xD000); // 53248

    // BX R2
    instance->get_mmu()->write16(CODE_INIT_ADDRESS + 12, 0x4710); // 18192

    // reset the cpu
    instance->reset();

    // run for 39 instructions
    instance->run(39);

    // grab the registers from the cpu
    arm_register_t* regs = instance->get_registers();

    // check the results
    EXPECT_EQ(regs[0].to_uint, 0);
    EXPECT_EQ(regs[1].to_uint, 1);

    // grab the program status register
    psr psr_register= instance->get_psr();

    // check it's state
    EXPECT_EQ(psr_register.n, false);
    EXPECT_EQ(psr_register.z, true);
    EXPECT_EQ(psr_register.c, false);
    EXPECT_EQ(psr_register.v, false);
    EXPECT_EQ(psr_register.t, true);
}