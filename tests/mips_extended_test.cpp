#include <gtest/gtest.h>
#include "../include/mips.h"
#include <vector>
#include <tuple>

// ============================================================================
// Parameterized Tests for MIPS Calculation
// ============================================================================

class MIPSCalculationTest : public ::testing::TestWithParam<std::tuple<int, int, long long, long long>> {};

TEST_P(MIPSCalculationTest, SingleInstructionAccumulation) {
  auto [num, time_ns, expected_total, expected_time] = GetParam();
  MIPS mips;
  mips.add(L"Test", num, time_ns);

  EXPECT_EQ(mips.total, expected_total);
  EXPECT_EQ(mips.total_time_in_ns, expected_time);
}

INSTANTIATE_TEST_SUITE_P(
  VariousInstructionCounts,
  MIPSCalculationTest,
  ::testing::Values(
    // (num_instructions, time_per_instruction_ns, expected_total, expected_total_time)
    std::make_tuple(1, 1, 1LL, 1LL),
    std::make_tuple(10, 10, 10LL, 100LL),
    std::make_tuple(100, 100, 100LL, 10000LL),
    std::make_tuple(1000, 50, 1000LL, 50000LL),
    std::make_tuple(5000, 200, 5000LL, 1000000LL),
    std::make_tuple(10000, 100, 10000LL, 1000000LL),
    std::make_tuple(50000, 75, 50000LL, 3750000LL),
    std::make_tuple(100000, 50, 100000LL, 5000000LL)
  )
);

// ============================================================================
// Parameterized Tests for MIPS Value Calculations
// ============================================================================

class MIPSValueTest : public ::testing::TestWithParam<std::tuple<long long, long long, double>> {};

TEST_P(MIPSValueTest, CorrectMIPSValue) {
  auto [total_instructions, total_time_ns, expected_mips] = GetParam();

  MIPS mips;
  mips.total = total_instructions;
  mips.total_time_in_ns = total_time_ns;

  double result = mips.get_mips();
  EXPECT_NEAR(result, expected_mips, 0.01);
}

INSTANTIATE_TEST_SUITE_P(
  KnownMIPSValues,
  MIPSValueTest,
  ::testing::Values(
    // (total_instructions, total_time_ns, expected_MIPS)
    // MIPS = (instructions / time_in_seconds) / 1,000,000
    std::make_tuple(1000000LL, 1000000000LL, 1.0),      // 1M instr / 1s = 1 MIPS
    std::make_tuple(2000000LL, 1000000000LL, 2.0),      // 2M instr / 1s = 2 MIPS
    std::make_tuple(5000000LL, 1000000000LL, 5.0),      // 5M instr / 1s = 5 MIPS
    std::make_tuple(10000000LL, 1000000000LL, 10.0),    // 10M instr / 1s = 10 MIPS
    std::make_tuple(100000000LL, 1000000000LL, 100.0),  // 100M instr / 1s = 100 MIPS
    std::make_tuple(500000LL, 500000000LL, 1.0),        // 500K instr / 0.5s = 1 MIPS
    std::make_tuple(1000000LL, 500000000LL, 2.0),       // 1M instr / 0.5s = 2 MIPS
    std::make_tuple(1000000LL, 100000000LL, 10.0),      // 1M instr / 0.1s = 10 MIPS
    std::make_tuple(50000LL, 10000000LL, 5.0),          // 50K instr / 0.01s = 5 MIPS
    std::make_tuple(10000LL, 1000000LL, 10.0)           // 10K instr / 0.001s = 10 MIPS
  )
);

// ============================================================================
// Real-World CPU Scenarios
// ============================================================================

TEST(MIPSRealWorldTest, Intel8086Scenario) {
  // Intel 8086 (1978): ~0.33 MIPS
  // 0.33 MIPS = 330,000 instructions per second
  // Time per instruction = 1 second / 330,000 ≈ 3030 ns
  MIPS mips;
  mips.add(L"8086 Instructions", 330000, 3030);

  double result = mips.get_mips();
  EXPECT_NEAR(result, 0.33, 0.01);
}

TEST(MIPSRealWorldTest, Intel80286Scenario) {
  // Intel 80286 (1982): ~1.5 MIPS
  // 1.5 MIPS = 1,500,000 instructions per second
  // Time per instruction = 1 second / 1,500,000 ≈ 667 ns
  MIPS mips;
  mips.add(L"80286 Instructions", 1500000, 667);

  double result = mips.get_mips();
  EXPECT_NEAR(result, 1.5, 0.05);
}

TEST(MIPSRealWorldTest, Intel80386Scenario) {
  // Intel 80386 (1985): ~5 MIPS
  // 5 MIPS = 5,000,000 instructions per second
  // Time per instruction = 1 second / 5,000,000 = 200 ns
  MIPS mips;
  mips.add(L"80386 Instructions", 5000000, 200);

  double result = mips.get_mips();
  EXPECT_NEAR(result, 5.0, 0.01);
}

TEST(MIPSRealWorldTest, Pentium1Scenario) {
  // Pentium (1993): ~100 MIPS
  // 100 MIPS = 100,000,000 instructions per second
  // Time per instruction = 1 second / 100,000,000 = 10 ns
  MIPS mips;
  mips.add(L"Pentium Instructions", 100000000, 10);

  double result = mips.get_mips();
  EXPECT_NEAR(result, 100.0, 0.1);
}

TEST(MIPSRealWorldTest, MixedWorkloadScenario) {
  // Realistic mixed workload
  MIPS mips;
  mips.add(L"Integer ALU", 40000, 50);        // Fast integer operations
  mips.add(L"Memory Load", 25000, 150);       // Memory loads
  mips.add(L"Memory Store", 20000, 140);      // Memory stores
  mips.add(L"Branch", 10000, 200);            // Branch operations
  mips.add(L"Multiply", 3000, 300);           // Multiply operations
  mips.add(L"Divide", 2000, 500);             // Divide operations (slowest)

  EXPECT_EQ(mips.total, 100000);
  // Total time calculation:
  // 40000*50 + 25000*150 + 20000*140 + 10000*200 + 3000*300 + 2000*500
  // = 2000000 + 3750000 + 2800000 + 2000000 + 900000 + 1000000 = 12450000 ns
  EXPECT_EQ(mips.total_time_in_ns, 12450000);

  // MIPS = 100000 / 0.012450 / 1000000 ≈ 8.03
  double result = mips.get_mips();
  EXPECT_NEAR(result, 8.03, 0.01);
}

TEST(MIPSRealWorldTest, MultimediaWorkload) {
  // Multimedia/graphics heavy workload
  MIPS mips;
  mips.add(L"Integer Ops", 50000, 40);
  mips.add(L"Float Add", 30000, 80);
  mips.add(L"Float Mul", 25000, 90);
  mips.add(L"SIMD", 15000, 60);
  mips.add(L"Memory", 20000, 120);

  EXPECT_EQ(mips.total, 140000);
  double result = mips.get_mips();
  EXPECT_GT(result, 0.0);
}

TEST(MIPSRealWorldTest, ServerWorkload) {
  // Server-type workload with many memory operations
  MIPS mips;
  mips.add(L"Cache Hits", 100000, 30);
  mips.add(L"Cache Miss", 10000, 200);
  mips.add(L"ALU Ops", 50000, 50);
  mips.add(L"Branch Pred Hit", 20000, 40);
  mips.add(L"Branch Pred Miss", 5000, 300);

  EXPECT_EQ(mips.total, 185000);
  double result = mips.get_mips();
  EXPECT_GT(result, 0.0);
}

// ============================================================================
// Stress Tests with Multiple Operations
// ============================================================================

TEST(MIPSStressTest, ManySmallOperations) {
  MIPS mips;

  // Add 100 small operations
  for (int i = 0; i < 100; i++) {
    mips.add(L"Op", 100, 10);
  }

  EXPECT_EQ(mips.total, 10000);
  EXPECT_EQ(mips.total_time_in_ns, 100000);

  double result = mips.get_mips();
  EXPECT_NEAR(result, 100.0, 0.1);
}

TEST(MIPSStressTest, VaryingInstructionCounts) {
  MIPS mips;

  // Add operations with linearly increasing counts
  for (int i = 1; i <= 50; i++) {
    mips.add(L"Op", i * 100, 50);
  }

  // Total: 100 + 200 + 300 + ... + 5000 = 127,500
  EXPECT_EQ(mips.total, 127500);

  double result = mips.get_mips();
  EXPECT_GT(result, 0.0);
}

TEST(MIPSStressTest, VaryingExecutionTimes) {
  MIPS mips;

  // Add operations with varying execution times
  for (int i = 1; i <= 20; i++) {
    mips.add(L"Op", 1000, i * 10);
  }

  EXPECT_EQ(mips.total, 20000);

  double result = mips.get_mips();
  EXPECT_GT(result, 0.0);
}

// ============================================================================
// Edge Cases and Boundary Conditions
// ============================================================================

TEST(MIPSEdgeCaseExtendedTest, SingleNanosecondInstructions) {
  MIPS mips;
  mips.add(L"Ultra Fast", 1000000, 1); // 1M instructions, 1ns each

  EXPECT_EQ(mips.total, 1000000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000); // 1ms total

  // MIPS = 1000000 / 0.001 / 1000000 = 1000
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1000.0);
}

TEST(MIPSEdgeCaseExtendedTest, OneSecondExecution) {
  MIPS mips;
  mips.add(L"One Second", 10000000, 100); // 10M instructions, 100ns each = 1s

  EXPECT_EQ(mips.total, 10000000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000000); // 1s

  // MIPS = 10000000 / 1.0 / 1000000 = 10
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 10.0);
}

TEST(MIPSEdgeCaseExtendedTest, OneMicrosecondInstructions) {
  MIPS mips;
  mips.add(L"Slow", 1000, 1000); // 1000 instructions, 1μs each

  EXPECT_EQ(mips.total, 1000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000); // 1ms

  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(MIPSEdgeCaseExtendedTest, MixedSpeedRatios) {
  MIPS mips;

  // 90% fast instructions, 10% slow
  mips.add(L"Fast", 90000, 10);
  mips.add(L"Slow", 10000, 1000);

  EXPECT_EQ(mips.total, 100000);
  // 90000*10 + 10000*1000 = 900000 + 10000000 = 10900000 ns
  EXPECT_EQ(mips.total_time_in_ns, 10900000);

  // MIPS = 100000 / 0.0109 / 1000000 ≈ 9.17
  double result = mips.get_mips();
  EXPECT_NEAR(result, 9.17, 0.01);
}

TEST(MIPSEdgeCaseExtendedTest, IdenticalOperations) {
  MIPS mips;

  // Add same operation multiple times
  for (int i = 0; i < 10; i++) {
    mips.add(L"Same", 1000, 100);
  }

  EXPECT_EQ(mips.total, 10000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000);
}

TEST(MIPSEdgeCaseExtendedTest, AlternatingFastSlow) {
  MIPS mips;

  // Alternate between fast and slow operations
  for (int i = 0; i < 5; i++) {
    mips.add(L"Fast", 1000, 10);
    mips.add(L"Slow", 100, 1000);
  }

  EXPECT_EQ(mips.total, 5500);
  // (1000*10 + 100*1000) * 5 = 110000 * 5 = 550000
  EXPECT_EQ(mips.total_time_in_ns, 550000);

  // MIPS = 5500 / 0.00055 / 1000000 = 10
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 10.0);
}

// ============================================================================
// Precision and Accuracy Tests
// ============================================================================

TEST(MIPSPrecisionTest, SmallNumberPrecision) {
  MIPS mips;
  mips.add(L"Test", 1, 1);

  EXPECT_EQ(mips.total, 1);
  EXPECT_EQ(mips.total_time_in_ns, 1);

  // MIPS = 1 / 0.000000001 / 1000000 = 1000
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1000.0);
}

TEST(MIPSPrecisionTest, PrimeNumberInputs) {
  MIPS mips;
  mips.add(L"Prime1", 7, 11);
  mips.add(L"Prime2", 13, 17);
  mips.add(L"Prime3", 19, 23);

  EXPECT_EQ(mips.total, 39);
  // 7*11 + 13*17 + 19*23 = 77 + 221 + 437 = 735
  EXPECT_EQ(mips.total_time_in_ns, 735);

  double result = mips.get_mips();
  // MIPS = 39 / 0.000000735 / 1000000 ≈ 53.06
  EXPECT_NEAR(result, 53.06, 0.1);
}

TEST(MIPSPrecisionTest, LargeButNotOverflow) {
  MIPS mips;
  mips.add(L"Large", 1000000, 5000); // 5 billion ns = 5 seconds

  EXPECT_EQ(mips.total, 1000000);
  EXPECT_EQ(mips.total_time_in_ns, 5000000000LL);

  // MIPS = 1000000 / 5.0 / 1000000 = 0.2
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 0.2);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
