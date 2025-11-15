cmake_minimum_required(VERSION 3.27)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files for libraries
set(BINARY32_SOURCES src/binary32.cpp)
set(MIPS_SOURCES src/mips.cpp)

# Create libraries
add_library(binary32_lib ${BINARY32_SOURCES})
add_library(mips_lib ${MIPS_SOURCES})

# Original executables
add_executable(binary32 binary32_main.cpp)
target_link_libraries(binary32 binary32_lib)

add_executable(mips mips_main.cpp)
target_link_libraries(mips mips_lib)

# Backward compatibility: keep original RISC_V executable
add_executable(RISC_V binary32.cpp)

# Enable testing
enable_testing()

# Download and configure Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Test executables
add_executable(binary32_test tests/binary32_test.cpp)
target_link_libraries(binary32_test binary32_lib gtest gtest_main)

add_executable(mips_test tests/mips_test.cpp)
target_link_libraries(mips_test mips_lib gtest gtest_main)

add_executable(binary32_extended_test tests/binary32_extended_test.cpp)
target_link_libraries(binary32_extended_test binary32_lib gtest gtest_main)

add_executable(mips_extended_test tests/mips_extended_test.cpp)
target_link_libraries(mips_extended_test mips_lib gtest gtest_main)

add_executable(integration_test tests/integration_test.cpp)
target_link_libraries(integration_test binary32_lib mips_lib gtest gtest_main)

# Register tests with CTest
add_test(NAME Binary32Tests COMMAND binary32_test)
add_test(NAME MIPSTests COMMAND mips_test)
add_test(NAME Binary32ExtendedTests COMMAND binary32_extended_test)
add_test(NAME MIPSExtendedTests COMMAND mips_extended_test)
add_test(NAME IntegrationTests COMMAND integration_test)

# Optional: Add a custom target to run all tests
add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
  DEPENDS binary32_test mips_test binary32_extended_test mips_extended_test integration_test
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
