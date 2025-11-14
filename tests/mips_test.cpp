#include <gtest/gtest.h>
#include "../include/mips.h"
#include <cmath>

// ============================================================================
// Tests for MIPS struct
// ============================================================================

TEST(MIPSTest, InitialValues) {
  MIPS mips;
  EXPECT_EQ(mips.total, 0);
  EXPECT_EQ(mips.total_time_in_ns, 0);
  EXPECT_DOUBLE_EQ(mips.get_mips(), 0.0);
}

TEST(MIPSTest, SingleInstructionType) {
  MIPS mips;
  mips.add(L"Test", 1000, 100); // 1000 instructions × 100ns
  EXPECT_EQ(mips.total, 1000);
  EXPECT_EQ(mips.total_time_in_ns, 100000); // 100,000 ns
}

TEST(MIPSTest, MultipleInstructionTypes) {
  MIPS mips;
  mips.add(L"Type1", 1000, 100);
  mips.add(L"Type2", 2000, 200);

  EXPECT_EQ(mips.total, 3000);
  EXPECT_EQ(mips.total_time_in_ns, 500000); // 100k + 400k = 500k ns
}

TEST(MIPSTest, MIPSCalculationSimple) {
  MIPS mips;
  // 1,000,000 instructions in 1 second = 1 MIPS
  mips.add(L"Test", 1000000, 1000); // 1M instructions × 1000ns = 1s
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(MIPSTest, MIPSCalculationFromExample) {
  // Test with the exact example from main.cpp
  MIPS mips;
  mips.add(L"Сложение", 23500, 90);         // 2,115,000 ns
  mips.add(L"Умножение", 30900, 160);       // 4,944,000 ns
  mips.add(L"Ветвление/прыжок", 15200, 250); // 3,800,000 ns
  mips.add(L"Другое", 8600, 60);            // 516,000 ns

  // Total: 78,200 instructions
  EXPECT_EQ(mips.total, 78200);

  // Total time: 11,375,000 ns = 0.011375 seconds
  EXPECT_EQ(mips.total_time_in_ns, 11375000);

  // MIPS = 78,200 / 0.011375 / 1,000,000 ≈ 6.875
  double result = mips.get_mips();
  EXPECT_NEAR(result, 6.875, 0.001);
}

TEST(MIPSTest, ZeroTimeHandling) {
  MIPS mips;
  // Should not crash with division by zero
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 0.0);
}

TEST(MIPSTest, LargeInstructionCounts) {
  MIPS mips;
  mips.add(L"Large", 1000000000, 10); // 1 billion instructions
  EXPECT_EQ(mips.total, 1000000000);
  EXPECT_EQ(mips.total_time_in_ns, 10000000000LL); // 10 billion ns
}

TEST(MIPSTest, AccumulationCorrectness) {
  MIPS mips;
  mips.add(L"A", 100, 10);   // 1,000 ns
  mips.add(L"B", 200, 20);   // 4,000 ns
  mips.add(L"C", 300, 30);   // 9,000 ns

  EXPECT_EQ(mips.total, 600);
  EXPECT_EQ(mips.total_time_in_ns, 14000);

  // MIPS = 600 / 0.000014 / 1,000,000 = 42.857...
  double result = mips.get_mips();
  EXPECT_NEAR(result, 42.857, 0.001);
}

TEST(MIPSTest, FastInstructions) {
  MIPS mips;
  // Very fast instructions (1 ns each)
  mips.add(L"Fast", 1000000, 1);

  EXPECT_EQ(mips.total, 1000000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000); // 1ms total

  // MIPS = 1,000,000 / 0.001 / 1,000,000 = 1000
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1000.0);
}

TEST(MIPSTest, SlowInstructions) {
  MIPS mips;
  // Very slow instructions (1000 ns each)
  mips.add(L"Slow", 1000, 1000);

  EXPECT_EQ(mips.total, 1000);
  EXPECT_EQ(mips.total_time_in_ns, 1000000); // 1ms total

  // MIPS = 1,000 / 0.001 / 1,000,000 = 1
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(MIPSTest, MixedInstructionSpeeds) {
  MIPS mips;
  mips.add(L"Fast", 10000, 1);    // 10,000 ns
  mips.add(L"Medium", 5000, 100); // 500,000 ns
  mips.add(L"Slow", 1000, 1000);  // 1,000,000 ns

  EXPECT_EQ(mips.total, 16000);
  EXPECT_EQ(mips.total_time_in_ns, 1510000); // 1.51 ms

  // MIPS = 16,000 / 0.00151 / 1,000,000 ≈ 10.596
  double result = mips.get_mips();
  EXPECT_NEAR(result, 10.596, 0.001);
}

TEST(MIPSTest, SingleInstruction) {
  MIPS mips;
  mips.add(L"Single", 1, 100);

  EXPECT_EQ(mips.total, 1);
  EXPECT_EQ(mips.total_time_in_ns, 100);

  // MIPS = 1 / 0.0000001 / 1,000,000 = 10
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 10.0);
}

TEST(MIPSTest, InstructionCountAccuracy) {
  MIPS mips;
  for (int i = 0; i < 10; i++) {
    mips.add(L"Instruction", 100, 10);
  }

  EXPECT_EQ(mips.total, 1000);
  EXPECT_EQ(mips.total_time_in_ns, 10000);
}

TEST(MIPSTest, TimeAccumulation) {
  MIPS mips;
  mips.add(L"A", 1, 1000);
  mips.add(L"B", 1, 2000);
  mips.add(L"C", 1, 3000);

  EXPECT_EQ(mips.total, 3);
  EXPECT_EQ(mips.total_time_in_ns, 6000);
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST(MIPSEdgeCaseTest, VeryHighMIPS) {
  MIPS mips;
  // Many instructions, very fast
  mips.add(L"Super Fast", 1000000000, 1); // 1B instructions, 1ns each

  // Total time: 1 second
  // MIPS = 1,000,000,000 / 1.0 / 1,000,000 = 1000
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1000.0);
}

TEST(MIPSEdgeCaseTest, VeryLowMIPS) {
  MIPS mips;
  // Few instructions, very slow
  mips.add(L"Super Slow", 1000, 1000000); // 1000 instructions, 1ms each

  // Total time: 1 second
  // MIPS = 1,000 / 1.0 / 1,000,000 = 0.001
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 0.001);
}

TEST(MIPSEdgeCaseTest, OverflowProtection) {
  MIPS mips;
  // Test with very large values
  mips.add(L"Large1", 2000000000, 100);
  mips.add(L"Large2", 2000000000, 100);

  // Should handle large numbers correctly
  EXPECT_EQ(mips.total, 4000000000LL);
  EXPECT_GT(mips.total_time_in_ns, 0);
}

TEST(MIPSEdgeCaseTest, RealWorldExample1) {
  // Modern CPU example: ~100,000 MIPS
  MIPS mips;
  mips.add(L"Modern CPU", 100000000, 1); // 100M instructions, 1ns each

  // Total time: 0.1 seconds
  // MIPS = 100,000,000 / 0.1 / 1,000,000 = 1000
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1000.0);
}

TEST(MIPSEdgeCaseTest, RealWorldExample2) {
  // Older CPU example: ~1 MIPS
  MIPS mips;
  mips.add(L"Old CPU", 1000000, 1000); // 1M instructions, 1000ns each

  // Total time: 1 second
  // MIPS = 1,000,000 / 1.0 / 1,000,000 = 1
  double result = mips.get_mips();
  EXPECT_DOUBLE_EQ(result, 1.0);
}

TEST(MIPSEdgeCaseTest, MultipleAddsWithSameType) {
  MIPS mips;
  // Adding same instruction type multiple times
  mips.add(L"Type A", 1000, 100);
  mips.add(L"Type A", 1000, 100);
  mips.add(L"Type A", 1000, 100);

  EXPECT_EQ(mips.total, 3000);
  EXPECT_EQ(mips.total_time_in_ns, 300000);
}

TEST(MIPSEdgeCaseTest, PrecisionTest) {
  MIPS mips;
  mips.add(L"Test", 12345, 67); // Odd numbers for precision testing

  EXPECT_EQ(mips.total, 12345);
  EXPECT_EQ(mips.total_time_in_ns, 827115);

  double result = mips.get_mips();
  // MIPS = 12345 / 0.000827115 / 1,000,000 ≈ 14.925
  EXPECT_NEAR(result, 14.925, 0.002);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
