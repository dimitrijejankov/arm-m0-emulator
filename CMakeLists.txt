cmake_minimum_required(VERSION 3.10)
project(emulator_m0)

# set the cpp standard to C++ 14
set(CMAKE_CXX_STANDARD 14)

#include pthreads
find_package(Threads)

# include the google test
include(GoogleTest)
find_package(GTest REQUIRED)

# add the include directories
include_directories("${PROJECT_SOURCE_DIR}/cpu")
include_directories("${PROJECT_SOURCE_DIR}/pheripherals")
include_directories("${PROJECT_SOURCE_DIR}/tests")

# create the main app
set(SOURCE_FILES cpu/mmu.cpp cpu/cpu.cpp cpu/cpu.cpp)
add_executable(emulator_m0 main.cpp ${SOURCE_FILES})

# create the mmu test
add_executable(TestMMU tests/test-mmu-test.cpp ${SOURCE_FILES})
target_link_libraries(TestMMU gtest_main gtest ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(TARGET TestMMU)

# create the cpu test
add_executable(TestCPU tests/test-cpu.cpp ${SOURCE_FILES})
target_link_libraries(TestCPU gtest_main gtest ${CMAKE_THREAD_LIBS_INIT})
gtest_add_tests(TARGET TestCPU)