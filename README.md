ARM M0 Emulator!
===================

Introduction
-------------

This is a implementation of the ARM M0 emulator, done as an individual project for COMP 554 at Rice University. The emulator is not cycle accurate and some features are not implemented. The ARM M0 specification is located at the following location :
http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0432c/CHDCICDF.html .

| Symbol | Parameters          | Description                                    | Status      |
|--------|---------------------|------------------------------------------------|-------------|
| ADCS   | {Rd,} Rn, Rm        | N,Z,C,V                                        | Implemented |
| ADD{S} | {Rd,} Rn, Rm&#124;#imm |Add                                       | Implemented |
| ADR    | Rd, label           | PC-relative Address to Register                | Implemented |
| ANDS   | {Rd,} Rn, Rm        | Bitwise AND                                    | Implemented |
| ASRS   | {Rd,} Rm, Rs&#124;#imm | Arithmetic Shift Right                         | Implemented |
| B{cc}  | label               | Branch {conditionally}                         | Implemented |
| BICS   | {Rd,} Rn, Rm        | Bit Clear                                      | Implemented |
| BKPT   | #imm                | Breakpoint                                     | Missing     |
| BL     | label               | Branch with Link                               | Missing     |
| BLX    | Rm                  | Branch indirect with Link                      | Implemented |
| BX     | Rm                  | Branch indirect                                | Implemented |
| CMN    | Rn, Rm              | Compare Negative                               | Implemented |
| CMP    | Rn, Rm&#124;#imm       | Compare                                        | Implemented |
| CPSID  | i                   | Change Processor State, Disable Interrupts     | Implemented |
| CPSIE  | i                   | Change Processor State, Enable Interrupts      | Implemented |
| DMB    | -                   | Data Memory Barrier                            | Missing     |
| DSB    | -                   | Data Synchronization Barrier                   | Missing     |
| EORS   | {Rd,} Rn, Rm        | Exclusive OR                                   | Implemented |
| ISB    | -                   | Instruction Synchronization Barrier            | Implemented |
| LDM    | Rn{!}, reglist      | Load Multiple registers, increment after       | Implemented |
| LDR    | Rt, label           | Load Register from PC-relative address         | Implemented |
| LDR    | Rt, [Rn, Rm&#124;#imm] | Load Register with word                        | Implemented |
| LDRB   | Rt, [Rn, Rm&#124;#imm] | Load Register with byte                        | Implemented |
| LDRH   | Rt, [Rn, Rm&#124;#imm] | Load Register with halfword                    | Implemented |
| LDRSB  | Rt, [Rn, Rm&#124;#imm] | Load Register with signed byte                 | Implemented |
| LDRSH  | Rt, [Rn, Rm&#124;#imm] | Load Register with signed halfword             | Implemented |
| LSLS   | {Rd,} Rn, Rs&#124;#imm | Logical Shift Left                             | Implemented |
| LSRS   | {Rd,} Rn, Rs&#124;#imm | Logical Shift Right                            | Implemented |
| MOV{S} | Rd, Rm              | Move                                           | Implemented |
| MRS    | Rd, spec_reg        | Move to general register from special register | Missing     |
| MSR    | spec_reg, Rm        | Move to special register from general register | Missing     |
| MULS   | Rd, Rn, Rm          | Multiply, 32-bit result                        | Implemented |
| MVNS   | Rd, Rm              | Bitwise NOT                                    | Implemented |
| NOP    | -                   | No Operation                                   | Implemented |
| ORRS   | {Rd,} Rn, Rm        | Logical OR                                     | Implemented |
| POP    | reglist             | Pop registers from stack                       | Implemented |
| PUSH   | reglist             | Push registers onto stack                      | Implemented |
| REV    | Rd, Rm              | Byte-Reverse word                              | Missing     |
| REV16  | Rd, Rm              | Byte-Reverse packed halfwords                  | Missing     |
| REVSH  | Rd, Rm              | Byte-Reverse signed halfword                   | Missing     |
| RORS   | {Rd,} Rn, Rs        | Rotate Right                                   | Implemented |
| RSBS   | {Rd,} Rn, #0        | Reverse Subtract                               | Missing     |
| SBCS   | {Rd,} Rn, Rm        | Subtract with Carry                            | Implemented |
| SEV    | -                   | Send Event                                     | Missing     |
| STM    | Rn!, reglist        | Store Multiple registers, increment after      | Implemented |
| STR    | Rt, [Rn, Rm&#124;#imm] | Store Register as word                         | Implemented |
| STRB   | Rt, [Rn, Rm&#124;#imm] | Store Register as byte                         | Implemented |
| STRH   | Rt, [Rn, Rm&#124;#imm] | Store Register as halfword                     | Implemented |
| SUB{S} | {Rd,} Rn, Rm&#124;#imm | Subtract                                       | Implemented |
| SVC    | #imm                | Supervisor Call                                | Missing     |
| SXTB   | Rd, Rm              | Sign extend byte                               | Missing     |
| SXTH   | Rd, Rm              | Sign extend halfword                           | Missing     |
| TST    | Rn, Rm              | Logical AND based test                         | Implemented |
| UXTB   | Rd, Rm              | Zero extend a byte                             | Missing     |
| UXTH   | Rd, Rm              | Zero extend a halfword                         | Missing     |
| WFE    | -                   | Wait For Event                                 | Missing     |
| WFI    | -                   | Wait For Interrupt                             | Missing     |

Usage
-------------
If you want to run your code you can do that from the command line. The the emulator takes in the arguments in the following form :
**emulator_m0** [-v] CODE_SIZE CODE_FILE SRAM_SIZE SRAM_FILE NUM_INSTR

| Symbol    | Description                                                                                       |
|-----------|---------------------------------------------------------------------------------------------------|
| -v        | This flag instructs the emulator to output extra information about the instructions it is running |
| CODE_SIZE | The size of the code region you are providing in **CODE_FILE**                                    |
| CODE_FILE | The file that contains the code region                                                            |
| SRAM_SIZE | The size of the SRAM the emulator has                                                             |
| SRAM_FILE | The file that contains the initial SRAM memory                                                    |
| NUM_INSTR | The number of instructions we want to run                                                         |

> **Example output:** 
> ***emulator_m0 -v 1024 examples/branches/code.bin 1024 examples/branches/sram.bin 3***
> Running in the verbose mode
> The starting PC : 58
> Executing instruction :200c
> Executing instruction :2101
> Executing instruction :467a
> 
> The state of the CPU
> The register 0 has value : c
> The register 1 has value : 1
> The register 2 has value : 60
> The register 3 has value : 0
> The register 4 has value : 0
> The register 5 has value : 0
> The register 6 has value : 0
> The register 7 has value : 0
> The register 8 has value : 0
> The register 9 has value : 0
> The register 10 has value : 0
> The register 11 has value : 0
> The register 12 has value : 0
> The register 13 has value : 0
> The register 14 has value : 0
> 
> T : 1
> C : 0
> N : 0
> V : 0
> 
Compiling
-------------------

The compiling is handled by CMake. The required version of CMake is 3.10, because it uses the new google test feature to specify the tests. The only dependency of the project is google test framework. You can find in the following link
https://github.com/google/googletest

> **Compiling commands** 
>  cd arm-m0-emulator
>  cmake .
>  make --target arm-m0-emulator
 
Running the Tests
-------------

In order to run the tests you first need to compile them with CMake.
> **Compiling commands**
> cd arm-m0-emulator
> cmake . --build ./build
> make --target TestCPU
> make --target TestMMU

Then go to your build directory and run :
> **Test Commands**
> cd build
> ./TestCPU
> ./TestMMU

Directory Structure
-------------
Below is the directory structure of the project :

| Directory    | Description                                                                       |
|--------------|-----------------------------------------------------------------------------------|
| cpu          | This directory contains the code of the cpu, with the instructions                |
| pheripherals | This directory contains the pheripherals, currently only the abstract pheripheral |
| tests        | This directory contains the google tests                                          |