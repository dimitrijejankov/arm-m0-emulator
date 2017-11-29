#include <iostream>
#include <climits>
#include <fstream>
#include <vector>
#include <queue>
#include <cpu.h>

int main(int argc, char *argv[]) {

    // by default we are not running in verbose mode.
    int verbose = 0;

    // are the parameters provided if not print help
    if (argc != 6 && argc != 7) {
        std::cout << "Usage: emulator_m0 [-v] CODE_SIZE CODE_FILE SRAM_SIZE SRAM_FILE NUM_INSTR" << std::endl;
        std::cout << std::endl;
        std::cout << "CODE_SIZE - has to be larger than 0" << std::endl;
        std::cout << "SRAM_SIZE - has to be larger than 0" << std::endl;
        std::cout << "NUM_INSTR - the number of instructions that need to be executed" << std::endl;
        return 0;
    }

    if(argc == 7 && std::string(argv[1]) == "-v") {
        std::cout << "Running in the verbose mode" << std::endl;
        verbose = true;
    }

    // grab the sizes
    auto code_size = std::strtoul(argv[1 + verbose], nullptr, 10);
    auto sram_size = std::strtoul(argv[3 + verbose], nullptr, 10);

    // check the code size
    if (code_size == 0 || code_size == ULONG_MAX) {
        std::cout << "CODE_SIZE is wrong" << std::endl;
        return -1;
    }

    // check the sram size
    if (sram_size == 0 || sram_size == ULONG_MAX) {
        std::cout << "SRAM_SIZE is wrong" << std::endl;
        return -1;
    }

    // allocate the code region
    auto *code_region = new uint8_t[code_size];

    // read the code region
    std::ifstream code_file(argv[2 + verbose], std::ios::binary);

    // check if we have opened the file
    if(!code_file.is_open()) {
        std::cout << "Could not open the " <<  argv[2 + verbose] << "file." << std::endl;
        return -1;
    }

    // copy the code region
    unsigned long i = 0;
    while (code_file.good() && i < code_size) {
        code_region[i++] = (uint8_t)code_file.get();
    }

    // close the file
    code_file.close();

    // allocate the sram region
    auto *sram_region = new uint8_t[sram_size];

    // read the sram region
    std::ifstream sram_file(argv[4 + verbose], std::ios::binary);

    // copy the sram region
    i = 0;
    while (sram_file.good() && i < code_size) {
        sram_region[i++] = (uint8_t)code_file.get();
    }

    // close the file
    sram_file.close();

    // create the cpu
    auto *instance = new cpu(code_region, sram_region);

    // number of instructions
    auto instr_num = std::strtoul(argv[5 + verbose], nullptr, 10);

    // run the cpu for a number of cycles
    if(!verbose) {
        instance->run(instr_num);
    }
    else {
        instance->verbose_run(instr_num);
    }

    // print the cpu status
    instance->print();

    return 0;
}
