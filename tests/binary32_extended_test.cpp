#include <gtest/gtest.h>
#include "../include/binary32.h"
#include <vector>
#include <tuple>

// ============================================================================
// Parameterized Tests for to_bin()
// ============================================================================

class ToBinParamTest : public ::testing::TestWithParam<std::tuple<int, int, std::wstring>> {};

TEST_P(ToBinParamTest, VariousInputs) {
  auto [value, length, expected] = GetParam();
  EXPECT_EQ(to_bin(value, length), expected);
}

INSTANTIATE_TEST_SUITE_P(
  BinaryConversions,
  ToBinParamTest,
  ::testing::Values(
    // (value, length, expected_binary)
    std::make_tuple(0, 1, L"0"),
    std::make_tuple(0, 8, L"00000000"),
    std::make_tuple(1, 8, L"00000001"),
    std::make_tuple(2, 8, L"00000010"),
    std::make_tuple(3, 8, L"00000011"),
    std::make_tuple(4, 8, L"00000100"),
    std::make_tuple(5, 8, L"00000101"),
    std::make_tuple(7, 8, L"00000111"),
    std::make_tuple(8, 8, L"00001000"),
    std::make_tuple(15, 8, L"00001111"),
    std::make_tuple(16, 8, L"00010000"),
    std::make_tuple(31, 8, L"00011111"),
    std::make_tuple(32, 8, L"00100000"),
    std::make_tuple(63, 8, L"00111111"),
    std::make_tuple(64, 8, L"01000000"),
    std::make_tuple(127, 8, L"01111111"),
    std::make_tuple(128, 8, L"10000000"),
    std::make_tuple(255, 8, L"11111111"),
    std::make_tuple(100, 8, L"01100100"),
    std::make_tuple(200, 8, L"11001000"),
    std::make_tuple(42, 8, L"00101010"),
    std::make_tuple(85, 8, L"01010101"),
    std::make_tuple(170, 8, L"10101010")
  )
);

// ============================================================================
// Parameterized Tests for IEEE 754 Exponents
// ============================================================================

class IEEE754ExponentTest : public ::testing::TestWithParam<std::tuple<float, int>> {};

TEST_P(IEEE754ExponentTest, CorrectExponent) {
  auto [value, expected_power] = GetParam();
  auto result = get_ieee754_components(value);
  EXPECT_EQ(result.power_of_2, expected_power);

  int expected_exponent = 127 + expected_power;
  EXPECT_EQ(result.exponent, to_bin(expected_exponent, 8));
}

INSTANTIATE_TEST_SUITE_P(
  PowersOfTwo,
  IEEE754ExponentTest,
  ::testing::Values(
    std::make_tuple(0.0625f, -4),   // 2^-4
    std::make_tuple(0.125f, -3),    // 2^-3
    std::make_tuple(0.25f, -2),     // 2^-2
    std::make_tuple(0.5f, -1),      // 2^-1
    std::make_tuple(1.0f, 0),       // 2^0
    std::make_tuple(2.0f, 1),       // 2^1
    std::make_tuple(4.0f, 2),       // 2^2
    std::make_tuple(8.0f, 3),       // 2^3
    std::make_tuple(16.0f, 4),      // 2^4
    std::make_tuple(32.0f, 5),      // 2^5
    std::make_tuple(64.0f, 6),      // 2^6
    std::make_tuple(128.0f, 7),     // 2^7
    std::make_tuple(256.0f, 8),     // 2^8
    std::make_tuple(512.0f, 9),     // 2^9
    std::make_tuple(1024.0f, 10)    // 2^10
  )
);

// ============================================================================
// Parameterized Tests for IEEE 754 Signs
// ============================================================================

class IEEE754SignTest : public ::testing::TestWithParam<std::tuple<float, std::wstring>> {};

TEST_P(IEEE754SignTest, CorrectSign) {
  auto [value, expected_sign] = GetParam();
  auto result = get_ieee754_components(value);
  EXPECT_EQ(result.sign, expected_sign);
}

INSTANTIATE_TEST_SUITE_P(
  PositiveAndNegative,
  IEEE754SignTest,
  ::testing::Values(
    std::make_tuple(0.0f, L"0"),
    std::make_tuple(1.0f, L"0"),
    std::make_tuple(2.5f, L"0"),
    std::make_tuple(100.0f, L"0"),
    std::make_tuple(0.125f, L"0"),
    std::make_tuple(-1.0f, L"1"),
    std::make_tuple(-2.5f, L"1"),
    std::make_tuple(-100.0f, L"1"),
    std::make_tuple(-0.125f, L"1"),
    std::make_tuple(-0.375f, L"1")
  )
);

// ============================================================================
// Additional IEEE 754 Edge Cases
// ============================================================================

TEST(IEEE754AdditionalTest, FractionalNumbers) {
  // 0.75 = 3/4 = 0.11 binary = 1.1 × 2^-1
  auto r1 = get_ieee754_components(0.75f);
  EXPECT_EQ(r1.sign, L"0");
  EXPECT_EQ(r1.power_of_2, -1);
  EXPECT_EQ(r1.mantissa.substr(0, 1), L"1");

  // 0.875 = 7/8 = 0.111 binary = 1.11 × 2^-1
  auto r2 = get_ieee754_components(0.875f);
  EXPECT_EQ(r2.sign, L"0");
  EXPECT_EQ(r2.power_of_2, -1);
  EXPECT_EQ(r2.mantissa.substr(0, 2), L"11");

  // 1.5 = 1.1 binary = 1.1 × 2^0
  auto r3 = get_ieee754_components(1.5f);
  EXPECT_EQ(r3.sign, L"0");
  EXPECT_EQ(r3.power_of_2, 0);
  EXPECT_EQ(r3.mantissa.substr(0, 1), L"1");

  // 1.25 = 1.01 binary = 1.01 × 2^0
  auto r4 = get_ieee754_components(1.25f);
  EXPECT_EQ(r4.sign, L"0");
  EXPECT_EQ(r4.power_of_2, 0);
  EXPECT_EQ(r4.mantissa.substr(0, 2), L"01");

  // 1.75 = 1.11 binary = 1.11 × 2^0
  auto r5 = get_ieee754_components(1.75f);
  EXPECT_EQ(r5.sign, L"0");
  EXPECT_EQ(r5.power_of_2, 0);
  EXPECT_EQ(r5.mantissa.substr(0, 2), L"11");
}

TEST(IEEE754AdditionalTest, LargerIntegers) {
  // 7 = 111 = 1.11 × 2^2
  auto r1 = get_ieee754_components(7.0f);
  EXPECT_EQ(r1.sign, L"0");
  EXPECT_EQ(r1.power_of_2, 2);

  // 15 = 1111 = 1.111 × 2^3
  auto r2 = get_ieee754_components(15.0f);
  EXPECT_EQ(r2.sign, L"0");
  EXPECT_EQ(r2.power_of_2, 3);

  // 31 = 11111 = 1.1111 × 2^4
  auto r3 = get_ieee754_components(31.0f);
  EXPECT_EQ(r3.sign, L"0");
  EXPECT_EQ(r3.power_of_2, 4);

  // 63 = 111111 = 1.11111 × 2^5
  auto r4 = get_ieee754_components(63.0f);
  EXPECT_EQ(r4.sign, L"0");
  EXPECT_EQ(r4.power_of_2, 5);

  // 127 = 1111111 = 1.111111 × 2^6
  auto r5 = get_ieee754_components(127.0f);
  EXPECT_EQ(r5.sign, L"0");
  EXPECT_EQ(r5.power_of_2, 6);
}

TEST(IEEE754AdditionalTest, MixedDecimalValues) {
  // 5.5 = 101.1 = 1.011 × 2^2
  auto r1 = get_ieee754_components(5.5f);
  EXPECT_EQ(r1.sign, L"0");
  EXPECT_EQ(r1.power_of_2, 2);
  EXPECT_EQ(r1.mantissa.substr(0, 3), L"011");

  // 6.75 = 110.11 = 1.1011 × 2^2
  auto r2 = get_ieee754_components(6.75f);
  EXPECT_EQ(r2.sign, L"0");
  EXPECT_EQ(r2.power_of_2, 2);
  EXPECT_EQ(r2.mantissa.substr(0, 4), L"1011");

  // 7.875 = 111.111 = 1.11111 × 2^2
  auto r3 = get_ieee754_components(7.875f);
  EXPECT_EQ(r3.sign, L"0");
  EXPECT_EQ(r3.power_of_2, 2);
  EXPECT_EQ(r3.mantissa.substr(0, 5), L"11111");

  // 10.5 = 1010.1 = 1.0101 × 2^3
  auto r4 = get_ieee754_components(10.5f);
  EXPECT_EQ(r4.sign, L"0");
  EXPECT_EQ(r4.power_of_2, 3);
  EXPECT_EQ(r4.mantissa.substr(0, 4), L"0101");
}

TEST(IEEE754AdditionalTest, SmallFractions) {
  // 0.03125 = 1/32 = 2^-5
  auto r1 = get_ieee754_components(0.03125f);
  EXPECT_EQ(r1.sign, L"0");
  EXPECT_EQ(r1.power_of_2, -5);

  // 0.015625 = 1/64 = 2^-6
  auto r2 = get_ieee754_components(0.015625f);
  EXPECT_EQ(r2.sign, L"0");
  EXPECT_EQ(r2.power_of_2, -6);

  // 0.1875 = 3/16 = 0.0011 = 1.1 × 2^-3
  auto r3 = get_ieee754_components(0.1875f);
  EXPECT_EQ(r3.sign, L"0");
  EXPECT_EQ(r3.power_of_2, -3);
  EXPECT_EQ(r3.mantissa.substr(0, 1), L"1");
}

TEST(IEEE754AdditionalTest, NegativeValues) {
  // -12.375 (same as positive but sign bit = 1)
  auto r1 = get_ieee754_components(-12.375f);
  EXPECT_EQ(r1.sign, L"1");
  EXPECT_EQ(r1.power_of_2, 3);
  EXPECT_EQ(r1.exponent, L"10000010");

  // -7.5 = -111.1 = -1.111 × 2^2
  auto r2 = get_ieee754_components(-7.5f);
  EXPECT_EQ(r2.sign, L"1");
  EXPECT_EQ(r2.power_of_2, 2);

  // -0.5
  auto r3 = get_ieee754_components(-0.5f);
  EXPECT_EQ(r3.sign, L"1");
  EXPECT_EQ(r3.power_of_2, -1);
}

TEST(IEEE754AdditionalTest, ComponentConsistency) {
  // Test that all components are consistent for various values
  std::vector<float> test_values = {
    0.5f, 1.0f, 1.5f, 2.0f, 3.0f, 4.0f, 5.0f,
    6.25f, 7.5f, 8.75f, 10.0f, 15.5f, 20.25f,
    0.125f, 0.25f, 0.375f, 0.625f, 0.875f
  };

  for (float value : test_values) {
    auto result = get_ieee754_components(value);

    // All components should have correct lengths
    EXPECT_EQ(result.sign.length(), 1) << "Failed for value: " << value;
    EXPECT_EQ(result.exponent.length(), 8) << "Failed for value: " << value;
    EXPECT_EQ(result.mantissa.length(), 23) << "Failed for value: " << value;

    // Sign should be 0 for positive
    EXPECT_EQ(result.sign, L"0") << "Failed for value: " << value;

    // All components should only contain 0s and 1s
    for (wchar_t c : result.sign + result.exponent + result.mantissa) {
      EXPECT_TRUE(c == L'0' || c == L'1') << "Failed for value: " << value;
    }
  }
}

TEST(IEEE754AdditionalTest, SymmetricValues) {
  // Test that positive and negative values have same exponent and mantissa
  std::vector<float> test_values = {1.0f, 2.5f, 7.25f, 0.375f, 12.375f};

  for (float value : test_values) {
    auto pos = get_ieee754_components(value);
    auto neg = get_ieee754_components(-value);

    EXPECT_EQ(pos.exponent, neg.exponent) << "Failed for value: " << value;
    EXPECT_EQ(pos.mantissa, neg.mantissa) << "Failed for value: " << value;
    EXPECT_NE(pos.sign, neg.sign) << "Failed for value: " << value;
    EXPECT_EQ(pos.sign, L"0") << "Failed for value: " << value;
    EXPECT_EQ(neg.sign, L"1") << "Failed for value: " << value;
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
