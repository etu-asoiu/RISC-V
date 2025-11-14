#include <gtest/gtest.h>
#include "../include/binary32.h"

// ============================================================================
// Tests for to_bin() function
// ============================================================================

TEST(ToBinTest, ZeroValue) {
  EXPECT_EQ(to_bin(0, 1), L"0");
  EXPECT_EQ(to_bin(0, 4), L"0000");
  EXPECT_EQ(to_bin(0, 8), L"00000000");
}

TEST(ToBinTest, SingleBitNumbers) {
  EXPECT_EQ(to_bin(1, 1), L"1");
  EXPECT_EQ(to_bin(1, 4), L"0001");
  EXPECT_EQ(to_bin(2, 4), L"0010");
  EXPECT_EQ(to_bin(4, 4), L"0100");
  EXPECT_EQ(to_bin(8, 4), L"1000");
}

TEST(ToBinTest, MultiBitNumbers) {
  EXPECT_EQ(to_bin(7, 4), L"0111");
  EXPECT_EQ(to_bin(15, 4), L"1111");
  EXPECT_EQ(to_bin(255, 8), L"11111111");
  EXPECT_EQ(to_bin(127, 8), L"01111111");
  EXPECT_EQ(to_bin(128, 8), L"10000000");
}

TEST(ToBinTest, PaddingBehavior) {
  EXPECT_EQ(to_bin(1, 8), L"00000001");
  EXPECT_EQ(to_bin(15, 8), L"00001111");
  EXPECT_EQ(to_bin(3, 10), L"0000000011");
}

TEST(ToBinTest, ExponentValues) {
  // Common IEEE 754 exponent values (bias = 127)
  EXPECT_EQ(to_bin(127, 8), L"01111111"); // exponent = 0
  EXPECT_EQ(to_bin(128, 8), L"10000000"); // exponent = 1
  EXPECT_EQ(to_bin(126, 8), L"01111110"); // exponent = -1
  EXPECT_EQ(to_bin(130, 8), L"10000010"); // exponent = 3
}

TEST(ToBinTest, LargeNumbers) {
  EXPECT_EQ(to_bin(1023, 10), L"1111111111");
  EXPECT_EQ(to_bin(512, 10), L"1000000000");
  EXPECT_EQ(to_bin(100, 8), L"01100100");
}

TEST(ToBinTest, PowersOfTwo) {
  EXPECT_EQ(to_bin(16, 8), L"00010000");
  EXPECT_EQ(to_bin(32, 8), L"00100000");
  EXPECT_EQ(to_bin(64, 8), L"01000000");
}

// ============================================================================
// Tests for get_ieee754_components() function
// ============================================================================

TEST(IEEE754Test, PositiveInteger) {
  auto result = get_ieee754_components(1.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111111"); // 127 (bias)
  EXPECT_EQ(result.mantissa.substr(0, 5), L"00000"); // Should be all zeros
  EXPECT_EQ(result.power_of_2, 0);
}

TEST(IEEE754Test, NegativeNumber) {
  auto result = get_ieee754_components(-1.0f);
  EXPECT_EQ(result.sign, L"1");
  EXPECT_EQ(result.exponent, L"01111111"); // 127
}

TEST(IEEE754Test, Zero) {
  auto result = get_ieee754_components(0.0f);
  EXPECT_EQ(result.sign, L"0");
}

TEST(IEEE754Test, PositiveTwo) {
  auto result = get_ieee754_components(2.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000000"); // 128 (2^1)
  EXPECT_EQ(result.power_of_2, 1);
}

TEST(IEEE754Test, PositiveFour) {
  auto result = get_ieee754_components(4.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000001"); // 129 (2^2)
  EXPECT_EQ(result.power_of_2, 2);
}

TEST(IEEE754Test, PositiveHalf) {
  auto result = get_ieee754_components(0.5f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111110"); // 126 (2^-1)
  EXPECT_EQ(result.power_of_2, -1);
}

TEST(IEEE754Test, PositiveQuarter) {
  auto result = get_ieee754_components(0.25f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111101"); // 125 (2^-2)
  EXPECT_EQ(result.power_of_2, -2);
}

TEST(IEEE754Test, Number12Point375) {
  // 12.375 from the original code
  // Binary: 1100.011
  // Normalized: 1.100011 × 2^3
  auto result = get_ieee754_components(12.375f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000010"); // 130 (127 + 3)
  EXPECT_EQ(result.power_of_2, 3);
  // Mantissa should start with 100011 (without leading 1)
  EXPECT_EQ(result.mantissa.substr(0, 6), L"100011");
}

TEST(IEEE754Test, NegativePoint375) {
  // -0.375 from the original code
  // Binary: -0.011
  // Normalized: -1.1 × 2^-2
  auto result = get_ieee754_components(-0.375f);
  EXPECT_EQ(result.sign, L"1");
  EXPECT_EQ(result.exponent, L"01111101"); // 125 (127 - 2)
  EXPECT_EQ(result.power_of_2, -2);
}

TEST(IEEE754Test, Point375) {
  // 0.375
  // Binary: 0.011
  // Normalized: 1.1 × 2^-2
  auto result = get_ieee754_components(0.375f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111101"); // 125 (127 - 2)
  EXPECT_EQ(result.power_of_2, -2);
  EXPECT_EQ(result.mantissa.substr(0, 1), L"1"); // Mantissa starts with 1
}

TEST(IEEE754Test, Point5) {
  // 0.5 = 1.0 × 2^-1
  auto result = get_ieee754_components(0.5f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111110"); // 126
  EXPECT_EQ(result.power_of_2, -1);
}

TEST(IEEE754Test, Point625) {
  // 0.625 = 0.101 = 1.01 × 2^-1
  auto result = get_ieee754_components(0.625f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111110"); // 126
  EXPECT_EQ(result.power_of_2, -1);
  EXPECT_EQ(result.mantissa.substr(0, 2), L"01");
}

TEST(IEEE754Test, Point125) {
  // 0.125 = 0.001 = 1.0 × 2^-3
  auto result = get_ieee754_components(0.125f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"01111100"); // 124 (127 - 3)
  EXPECT_EQ(result.power_of_2, -3);
}

TEST(IEEE754Test, Number255) {
  // 255 = 11111111 = 1.1111111 × 2^7
  auto result = get_ieee754_components(255.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000110"); // 134 (127 + 7)
  EXPECT_EQ(result.power_of_2, 7);
}

TEST(IEEE754Test, Negative255) {
  auto result = get_ieee754_components(-255.0f);
  EXPECT_EQ(result.sign, L"1");
  EXPECT_EQ(result.exponent, L"10000110"); // 134 (127 + 7)
  EXPECT_EQ(result.power_of_2, 7);
}

TEST(IEEE754Test, Number8) {
  // 8 = 1000 = 1.0 × 2^3
  auto result = get_ieee754_components(8.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000010"); // 130 (127 + 3)
  EXPECT_EQ(result.power_of_2, 3);
}

TEST(IEEE754Test, Number16) {
  // 16 = 10000 = 1.0 × 2^4
  auto result = get_ieee754_components(16.0f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000011"); // 131 (127 + 4)
  EXPECT_EQ(result.power_of_2, 4);
}

TEST(IEEE754Test, Number3Point625) {
  // 3.625 = 11.101 = 1.1101 × 2^1
  auto result = get_ieee754_components(3.625f);
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10000000"); // 128 (127 + 1)
  EXPECT_EQ(result.power_of_2, 1);
  EXPECT_EQ(result.mantissa.substr(0, 4), L"1101");
}

TEST(IEEE754Test, MantissaLength) {
  // All mantissas should be exactly 23 bits
  auto result1 = get_ieee754_components(1.0f);
  EXPECT_EQ(result1.mantissa.length(), 23);

  auto result2 = get_ieee754_components(12.375f);
  EXPECT_EQ(result2.mantissa.length(), 23);

  auto result3 = get_ieee754_components(0.375f);
  EXPECT_EQ(result3.mantissa.length(), 23);
}

TEST(IEEE754Test, ExponentLength) {
  // All exponents should be exactly 8 bits
  auto result1 = get_ieee754_components(1.0f);
  EXPECT_EQ(result1.exponent.length(), 8);

  auto result2 = get_ieee754_components(255.0f);
  EXPECT_EQ(result2.exponent.length(), 8);

  auto result3 = get_ieee754_components(0.125f);
  EXPECT_EQ(result3.exponent.length(), 8);
}

TEST(IEEE754Test, SignLength) {
  // All signs should be exactly 1 bit
  auto result1 = get_ieee754_components(1.0f);
  EXPECT_EQ(result1.sign.length(), 1);

  auto result2 = get_ieee754_components(-1.0f);
  EXPECT_EQ(result2.sign.length(), 1);
}

// ============================================================================
// Edge case tests
// ============================================================================

TEST(IEEE754EdgeCaseTest, VerySmallPositive) {
  // Test with small fractions
  auto result = get_ieee754_components(0.0625f); // 2^-4
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.power_of_2, -4);
}

TEST(IEEE754EdgeCaseTest, VerySmallNegative) {
  auto result = get_ieee754_components(-0.0625f); // -2^-4
  EXPECT_EQ(result.sign, L"1");
  EXPECT_EQ(result.power_of_2, -4);
}

TEST(IEEE754EdgeCaseTest, LargePowerOfTwo) {
  auto result = get_ieee754_components(1024.0f); // 2^10
  EXPECT_EQ(result.sign, L"0");
  EXPECT_EQ(result.exponent, L"10001001"); // 137 (127 + 10)
  EXPECT_EQ(result.power_of_2, 10);
}

TEST(IEEE754EdgeCaseTest, ComponentsAreBinary) {
  auto result = get_ieee754_components(12.375f);

  // Check that all characters in components are '0' or '1'
  for (wchar_t c : result.sign) {
    EXPECT_TRUE(c == L'0' || c == L'1');
  }
  for (wchar_t c : result.exponent) {
    EXPECT_TRUE(c == L'0' || c == L'1');
  }
  for (wchar_t c : result.mantissa) {
    EXPECT_TRUE(c == L'0' || c == L'1');
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
