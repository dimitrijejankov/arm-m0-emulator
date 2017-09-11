cmake_minimum_required(VERSION 3.8)
project(emulator_m0)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp instructions/codes.h instructions/conditions.h instructions/instructions.h instructions/encoder.cpp instructions/encoder.h cpu/cpu.cpp cpu/cpu.h cpu/registers.h cpu/cpu.cpp cpu/cpu.h)
add_executable(emulator_m0 ${SOURCE_FILES})