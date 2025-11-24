#include <gtest/gtest.h>

#include <cmath>

#include "mips.h"

using namespace mips;

// ============================================================================
// Tests for MIPSCalculator class
// ============================================================================

class MIPSCalculatorTest : public ::testing::Test {
 protected:
  MIPSCalculator calculator;

  void SetUp() override { calculator.reset(); }
};

// Test initial state
TEST_F(MIPSCalculatorTest, InitialState) {
  EXPECT_EQ(calculator.get_total_instructions(), 0);
  EXPECT_EQ(calculator.get_total_time_ns(), 0);
  EXPECT_DOUBLE_EQ(calculator.get_total_time_sec(), 0.0);
  EXPECT_DOUBLE_EQ(calculator.calculate_mips(), 0.0);
  EXPECT_TRUE(calculator.get_instructions().empty());
}

// Test adding single instruction type
TEST_F(MIPSCalculatorTest, AddSingleInstruction) {
  calculator.add(L"ADD", 1000, 100);

  EXPECT_EQ(calculator.get_total_instructions(), 1000);
  EXPECT_EQ(calculator.get_total_time_ns(), 100000);  // 1000 * 100
  EXPECT_EQ(calculator.get_instructions().size(), 1u);
}

// Test adding multiple instruction types
TEST_F(MIPSCalculatorTest, AddMultipleInstructions) {
  calculator.add(L"ADD", 1000, 100);
  calculator.add(L"MUL", 500, 200);
  calculator.add(L"BRANCH", 250, 150);

  EXPECT_EQ(calculator.get_total_instructions(), 1750);  // 1000 + 500 + 250
  // Total time: 1000*100 + 500*200 + 250*150 = 100000 + 100000 + 37500 = 237500
  EXPECT_EQ(calculator.get_total_time_ns(), 237500);
  EXPECT_EQ(calculator.get_instructions().size(), 3u);
}

// Test MIPS calculation - basic case
TEST_F(MIPSCalculatorTest, CalculateMIPSBasic) {
  // 1,000,000 instructions × 1000 ns each = 1,000,000,000 ns = 1 second total
  // MIPS = (1,000,000 / 1.0) / 1,000,000 = 1.0 MIPS
  calculator.add(L"ADD", 1000000, 1000);

  double mips = calculator.calculate_mips();
  EXPECT_NEAR(mips, 1.0, 0.001);
}

// Test MIPS calculation - from original code example
TEST_F(MIPSCalculatorTest, CalculateMIPSOriginalExample) {
  // Original test data from main.cpp
  calculator.add(L"Addition", 23500, 90);
  calculator.add(L"Multiplication", 30900, 160);
  calculator.add(L"Branch/Jump", 15200, 250);
  calculator.add(L"Other", 8600, 60);

  long long expected_total_instructions = 23500 + 30900 + 15200 + 8600;
  EXPECT_EQ(calculator.get_total_instructions(), expected_total_instructions);
  EXPECT_EQ(expected_total_instructions, 78200);

  // Total time calculation
  long long expected_total_time =
      (23500LL * 90) + (30900LL * 160) + (15200LL * 250) + (8600LL * 60);
  EXPECT_EQ(calculator.get_total_time_ns(), expected_total_time);
  // 2115000 + 4944000 + 3800000 + 516000 = 11375000 ns
  EXPECT_EQ(expected_total_time, 11375000);

  // MIPS calculation
  double total_time_sec = expected_total_time * 1e-9;
  double expected_mips =
      (static_cast<double>(expected_total_instructions) / total_time_sec) / 1e6;

  double calculated_mips = calculator.calculate_mips();
  EXPECT_NEAR(calculated_mips, expected_mips, 0.0001);
}

// Test time conversion
TEST_F(MIPSCalculatorTest, TimeConversion) {
  calculator.add(L"ADD", 1000, 1000000);  // 1000 * 1,000,000 ns = 1,000,000,000
                                          // ns = 1 sec

  EXPECT_EQ(calculator.get_total_time_ns(), 1000000000);
  EXPECT_DOUBLE_EQ(calculator.get_total_time_sec(), 1.0);
}

// Test reset functionality
TEST_F(MIPSCalculatorTest, Reset) {
  calculator.add(L"ADD", 1000, 100);
  calculator.add(L"MUL", 500, 200);

  calculator.reset();

  EXPECT_EQ(calculator.get_total_instructions(), 0);
  EXPECT_EQ(calculator.get_total_time_ns(), 0);
  EXPECT_TRUE(calculator.get_instructions().empty());
}

// Test with zero time (edge case)
TEST_F(MIPSCalculatorTest, ZeroTimeReturnsZeroMIPS) {
  // Don't add any instructions - time is 0
  EXPECT_DOUBLE_EQ(calculator.calculate_mips(), 0.0);
}

// Test instruction metrics stored correctly
TEST_F(MIPSCalculatorTest, InstructionMetricsStored) {
  calculator.add(L"TestInstr", 100, 50);

  const auto& instructions = calculator.get_instructions();
  ASSERT_EQ(instructions.size(), 1u);
  EXPECT_EQ(instructions[0].name, L"TestInstr");
  EXPECT_EQ(instructions[0].count, 100);
  EXPECT_EQ(instructions[0].time_in_ns, 50);
  EXPECT_EQ(instructions[0].total_time_ns, 5000);  // 100 * 50
}

// Test with large numbers
TEST_F(MIPSCalculatorTest, LargeNumbers) {
  // Simulate a real processor workload
  calculator.add(L"ADD", 1000000000, 1);  // 1 billion instructions, 1ns each

  EXPECT_EQ(calculator.get_total_instructions(), 1000000000);
  EXPECT_EQ(calculator.get_total_time_ns(), 1000000000);

  // MIPS = (1,000,000,000 / 1.0) / 1,000,000 = 1000 MIPS
  EXPECT_NEAR(calculator.calculate_mips(), 1000.0, 0.001);
}

// Test single instruction
TEST_F(MIPSCalculatorTest, SingleInstruction) {
  calculator.add(L"NOP", 1, 1);

  EXPECT_EQ(calculator.get_total_instructions(), 1);
  EXPECT_EQ(calculator.get_total_time_ns(), 1);

  // MIPS = (1 / 0.000000001) / 1,000,000 = 1000 MIPS
  EXPECT_NEAR(calculator.calculate_mips(), 1000.0, 0.001);
}

// Test accumulation accuracy
TEST_F(MIPSCalculatorTest, AccumulationAccuracy) {
  for (int i = 0; i < 100; i++) {
    calculator.add(L"INSTR", 10, 10);
  }

  EXPECT_EQ(calculator.get_total_instructions(), 1000);
  EXPECT_EQ(calculator.get_total_time_ns(), 10000);
  EXPECT_EQ(calculator.get_instructions().size(), 100u);
}

// Test different instruction time values
TEST_F(MIPSCalculatorTest, DifferentInstructionTimes) {
  calculator.add(L"FAST", 1000, 1);    // 1ns per instruction
  calculator.add(L"MEDIUM", 1000, 10);  // 10ns per instruction
  calculator.add(L"SLOW", 1000, 100);   // 100ns per instruction

  EXPECT_EQ(calculator.get_total_instructions(), 3000);
  // Total time: 1000*1 + 1000*10 + 1000*100 = 1000 + 10000 + 100000 = 111000
  EXPECT_EQ(calculator.get_total_time_ns(), 111000);
}

// Test MIPS formula correctness
TEST_F(MIPSCalculatorTest, MIPSFormulaCorrectness) {
  // Set up known values for easy verification
  // 1000 instructions at 1000ns each = 1,000,000 ns = 0.001 sec total
  calculator.add(L"TEST", 1000, 1000);

  double total_time_sec = calculator.get_total_time_sec();
  EXPECT_DOUBLE_EQ(total_time_sec, 0.001);

  // MIPS = (1000 / 0.001) / 1,000,000 = 1,000,000 / 1,000,000 = 1.0
  EXPECT_NEAR(calculator.calculate_mips(), 1.0, 0.0001);
}

// Test with very small instruction counts
TEST_F(MIPSCalculatorTest, SmallInstructionCounts) {
  calculator.add(L"ADD", 1, 1);
  calculator.add(L"SUB", 1, 1);
  calculator.add(L"MUL", 1, 1);

  EXPECT_EQ(calculator.get_total_instructions(), 3);
  EXPECT_EQ(calculator.get_total_time_ns(), 3);
}

// Test precision of MIPS calculation
TEST_F(MIPSCalculatorTest, MIPSPrecision) {
  // Use values that should give exactly 6.875 MIPS
  // If 78200 instructions take 11375000 ns
  // MIPS = (78200 / 0.011375) / 1e6 = 6.875...
  calculator.add(L"Addition", 23500, 90);
  calculator.add(L"Multiplication", 30900, 160);
  calculator.add(L"Branch/Jump", 15200, 250);
  calculator.add(L"Other", 8600, 60);

  double mips = calculator.calculate_mips();
  // Expected: 78200 / 0.011375 / 1000000 ≈ 6.8747...
  EXPECT_GT(mips, 6.8);
  EXPECT_LT(mips, 7.0);
}

// ============================================================================
// Tests for InstructionMetrics struct
// ============================================================================

class InstructionMetricsTest : public ::testing::Test {};

TEST_F(InstructionMetricsTest, DefaultValues) {
  InstructionMetrics metrics;
  // Default constructed values should be zero-initialized or default
  EXPECT_EQ(metrics.count, 0);
  EXPECT_EQ(metrics.time_in_ns, 0);
  EXPECT_EQ(metrics.total_time_ns, 0);
}

TEST_F(InstructionMetricsTest, AssignmentWorks) {
  InstructionMetrics metrics;
  metrics.name = L"TestInstruction";
  metrics.count = 100;
  metrics.time_in_ns = 50;
  metrics.total_time_ns = 5000;

  EXPECT_EQ(metrics.name, L"TestInstruction");
  EXPECT_EQ(metrics.count, 100);
  EXPECT_EQ(metrics.time_in_ns, 50);
  EXPECT_EQ(metrics.total_time_ns, 5000);
}
