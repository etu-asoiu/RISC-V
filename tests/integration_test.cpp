#include <gtest/gtest.h>
#include "../include/binary32.h"
#include "../include/mips.h"
#include <sstream>
#include <vector>

// ============================================================================
// Integration Tests - Testing Multiple Components Together
// ============================================================================

TEST(IntegrationTest, BinaryConversionConsistency) {
  // Test that to_bin produces consistent results for IEEE754 exponents
  for (int exp = 0; exp <= 255; exp++) {
    wstring bin = to_bin(exp, 8);

    // Should always be 8 characters
    EXPECT_EQ(bin.length(), 8);

    // Should only contain 0 and 1
    for (wchar_t c : bin) {
      EXPECT_TRUE(c == L'0' || c == L'1');
    }

    // Converting back should give same value (manual verification)
    int reconstructed = 0;
    for (size_t i = 0; i < bin.length(); i++) {
      if (bin[i] == L'1') {
        reconstructed += (1 << (7 - i));
      }
    }
    EXPECT_EQ(reconstructed, exp);
  }
}

TEST(IntegrationTest, IEEE754ComponentsFullCycle) {
  // Test complete IEEE 754 representation cycle
  std::vector<float> test_values = {
    1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 10.0f, 100.0f,
    0.5f, 0.25f, 0.125f, 0.75f, 1.5f, 2.5f,
    12.375f, 3.625f, 7.875f, -0.375f, -12.375f
  };

  for (float value : test_values) {
    auto components = get_ieee754_components(value);

    // Verify total bit count = 32 (1 + 8 + 23)
    size_t total_bits = components.sign.length() +
                       components.exponent.length() +
                       components.mantissa.length();
    EXPECT_EQ(total_bits, 32) << "Failed for value: " << value;

    // Verify sign correctness
    if (value >= 0) {
      EXPECT_EQ(components.sign, L"0") << "Failed for value: " << value;
    } else {
      EXPECT_EQ(components.sign, L"1") << "Failed for value: " << value;
    }

    // Verify exponent is valid (not all 0s or all 1s for normal numbers)
    EXPECT_NE(components.exponent, L"00000000") << "Failed for value: " << value;
    EXPECT_NE(components.exponent, L"11111111") << "Failed for value: " << value;
  }
}

TEST(IntegrationTest, MIPSWithRealisticProfile) {
  // Simulate a realistic instruction mix based on actual CPU studies
  // Typical instruction distribution:
  // ~25% ALU operations
  // ~25% Load operations
  // ~15% Store operations
  // ~25% Branch operations
  // ~10% Other

  MIPS mips;

  // For 100,000 total instructions:
  mips.add(L"ALU", 25000, 50);      // 25%
  mips.add(L"Load", 25000, 120);    // 25%
  mips.add(L"Store", 15000, 110);   // 15%
  mips.add(L"Branch", 25000, 80);   // 25%
  mips.add(L"Other", 10000, 60);    // 10%

  EXPECT_EQ(mips.total, 100000);

  // Calculate expected time:
  // 25000*50 + 25000*120 + 15000*110 + 25000*80 + 10000*60
  // = 1250000 + 3000000 + 1650000 + 2000000 + 600000 = 8500000 ns
  EXPECT_EQ(mips.total_time_in_ns, 8500000);

  double result = mips.get_mips();
  // MIPS = 100000 / 0.0085 / 1000000 ≈ 11.76
  EXPECT_NEAR(result, 11.76, 0.01);
}

TEST(IntegrationTest, BinaryAndIEEE754Exponents) {
  // Test that exponents in IEEE754 match to_bin output
  std::vector<std::pair<float, int>> test_cases = {
    {1.0f, 127},    // 2^0 -> exponent = 127
    {2.0f, 128},    // 2^1 -> exponent = 128
    {4.0f, 129},    // 2^2 -> exponent = 129
    {0.5f, 126},    // 2^-1 -> exponent = 126
    {0.25f, 125},   // 2^-2 -> exponent = 125
  };

  for (auto [value, expected_exp] : test_cases) {
    auto components = get_ieee754_components(value);
    wstring expected_bin = to_bin(expected_exp, 8);

    EXPECT_EQ(components.exponent, expected_bin)
      << "Failed for value: " << value
      << ", expected exponent: " << expected_exp;
  }
}

TEST(IntegrationTest, MultipleConversionsIndependence) {
  // Test that multiple conversions don't affect each other
  auto r1 = get_ieee754_components(12.375f);
  auto r2 = get_ieee754_components(3.625f);
  auto r3 = get_ieee754_components(12.375f); // Same as r1

  // r1 and r3 should be identical
  EXPECT_EQ(r1.sign, r3.sign);
  EXPECT_EQ(r1.exponent, r3.exponent);
  EXPECT_EQ(r1.mantissa, r3.mantissa);
  EXPECT_EQ(r1.power_of_2, r3.power_of_2);

  // r1 and r2 should be different
  EXPECT_NE(r1.exponent, r2.exponent);
  EXPECT_NE(r1.mantissa, r2.mantissa);
}

TEST(IntegrationTest, MIPSAccumulationOrder) {
  // Test that order of addition doesn't matter (commutative)
  MIPS mips1, mips2;

  mips1.add(L"A", 1000, 100);
  mips1.add(L"B", 2000, 200);
  mips1.add(L"C", 3000, 300);

  mips2.add(L"C", 3000, 300);
  mips2.add(L"A", 1000, 100);
  mips2.add(L"B", 2000, 200);

  EXPECT_EQ(mips1.total, mips2.total);
  EXPECT_EQ(mips1.total_time_in_ns, mips2.total_time_in_ns);
  EXPECT_DOUBLE_EQ(mips1.get_mips(), mips2.get_mips());
}

TEST(IntegrationTest, BinaryPaddingConsistency) {
  // Test that padding works consistently across different lengths
  for (int len = 1; len <= 16; len++) {
    wstring bin = to_bin(1, len);
    EXPECT_EQ(bin.length(), len);
    EXPECT_EQ(bin[len - 1], L'1'); // Last bit should be 1
    for (int i = 0; i < len - 1; i++) {
      EXPECT_EQ(bin[i], L'0'); // All other bits should be 0
    }
  }
}

TEST(IntegrationTest, IEEE754SymmetryCheck) {
  // Test symmetry between positive and negative numbers
  std::vector<float> positive_values = {
    1.0f, 2.5f, 7.25f, 12.375f, 0.375f, 0.125f, 16.0f, 3.625f
  };

  for (float pos_val : positive_values) {
    float neg_val = -pos_val;

    auto pos = get_ieee754_components(pos_val);
    auto neg = get_ieee754_components(neg_val);

    // Only sign bit should differ
    EXPECT_NE(pos.sign, neg.sign);
    EXPECT_EQ(pos.exponent, neg.exponent);
    EXPECT_EQ(pos.mantissa, neg.mantissa);
    EXPECT_EQ(pos.power_of_2, neg.power_of_2);
  }
}

TEST(IntegrationTest, MIPSWithIncrementalBuildup) {
  // Test building up MIPS calculation incrementally
  MIPS mips;

  // Start with base load
  mips.add(L"Base", 10000, 100);
  double mips1 = mips.get_mips();

  // Add more
  mips.add(L"Additional", 10000, 100);
  double mips2 = mips.get_mips();

  // MIPS should stay the same if instruction mix is identical
  EXPECT_DOUBLE_EQ(mips1, mips2);

  // Add slower instructions
  mips.add(L"Slow", 10000, 200);
  double mips3 = mips.get_mips();

  // MIPS should decrease when slower instructions added
  EXPECT_LT(mips3, mips2);
}

TEST(IntegrationTest, ComplexWorkloadSimulation) {
  // Simulate a complex multi-phase workload
  MIPS mips;

  // Phase 1: Initialization (fast)
  for (int i = 0; i < 5; i++) {
    mips.add(L"Init", 1000, 50);
  }

  // Phase 2: Main computation (mixed speed)
  for (int i = 0; i < 10; i++) {
    mips.add(L"Compute", 500, 100 + i * 10);
  }

  // Phase 3: Memory operations (slower)
  for (int i = 0; i < 3; i++) {
    mips.add(L"Memory", 800, 200);
  }

  // Verify totals
  // Phase 1: 5 * 1000 = 5000
  // Phase 2: 10 * 500 = 5000
  // Phase 3: 3 * 800 = 2400
  // Total: 12400
  EXPECT_EQ(mips.total, 12400);

  double result = mips.get_mips();
  EXPECT_GT(result, 0.0);
}

TEST(IntegrationTest, BinaryConversionEdgeLengths) {
  // Test binary conversion with various practical lengths used in IEEE754
  struct TestCase {
    int value;
    int length;
    wstring expected;
  };

  std::vector<TestCase> cases = {
    {0, 1, L"0"},
    {1, 1, L"1"},
    {1, 8, L"00000001"},
    {127, 8, L"01111111"},  // Common IEEE754 bias
    {128, 8, L"10000000"},
    {255, 8, L"11111111"},
    {1, 23, L"00000000000000000000001"},  // Mantissa length
    {0, 23, L"00000000000000000000000"},
  };

  for (const auto& tc : cases) {
    EXPECT_EQ(to_bin(tc.value, tc.length), tc.expected)
      << "Failed for value=" << tc.value << ", length=" << tc.length;
  }
}

TEST(IntegrationTest, FullSystemExample) {
  // Complete example using all components together

  // Part 1: Convert some floats to IEEE754
  std::vector<float> values = {1.0f, 2.0f, 4.0f, 8.0f};
  std::vector<IEEE754Components> components;

  for (float val : values) {
    components.push_back(get_ieee754_components(val));
  }

  // Verify all conversions succeeded
  EXPECT_EQ(components.size(), 4);
  for (const auto& comp : components) {
    EXPECT_EQ(comp.sign.length(), 1);
    EXPECT_EQ(comp.exponent.length(), 8);
    EXPECT_EQ(comp.mantissa.length(), 23);
  }

  // Part 2: Simulate MIPS calculation for processing these values
  MIPS mips;
  mips.add(L"Float Conversion", values.size(), 150);
  mips.add(L"Post-processing", values.size(), 50);

  EXPECT_EQ(mips.total, 8);
  EXPECT_EQ(mips.total_time_in_ns, 800);

  double result = mips.get_mips();
  // MIPS = 8 / 0.0000008 / 1000000 = 10
  EXPECT_DOUBLE_EQ(result, 10.0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
