cmake_minimum_required(VERSION 3.14)
project(RISC_V)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executables
add_executable(RISC_V binary32.cpp)
add_executable(MIPS_Calculator main.cpp)

# Testing configuration
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
  )
  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  FetchContent_MakeAvailable(googletest)

  enable_testing()

  # Test executable
  add_executable(run_tests
    tests/test_binary32.cpp
    tests/test_mips.cpp
  )

  target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include)

  target_link_libraries(run_tests
    GTest::gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(run_tests)
endif()
