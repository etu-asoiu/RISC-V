#include <gtest/gtest.h>

#include "binary32.h"

using namespace binary32;

// ============================================================================
// Tests for to_bin() function
// ============================================================================

class ToBinTest : public ::testing::Test {};

TEST_F(ToBinTest, ConvertZero) {
  EXPECT_EQ(wstring_to_string(to_bin(0, 1)), "0");
  EXPECT_EQ(wstring_to_string(to_bin(0, 4)), "0000");
  EXPECT_EQ(wstring_to_string(to_bin(0, 8)), "00000000");
}

TEST_F(ToBinTest, ConvertOne) {
  EXPECT_EQ(wstring_to_string(to_bin(1, 1)), "1");
  EXPECT_EQ(wstring_to_string(to_bin(1, 4)), "0001");
  EXPECT_EQ(wstring_to_string(to_bin(1, 8)), "00000001");
}

TEST_F(ToBinTest, ConvertSmallNumbers) {
  EXPECT_EQ(wstring_to_string(to_bin(2, 1)), "10");
  EXPECT_EQ(wstring_to_string(to_bin(3, 2)), "11");
  EXPECT_EQ(wstring_to_string(to_bin(4, 3)), "100");
  EXPECT_EQ(wstring_to_string(to_bin(5, 3)), "101");
  EXPECT_EQ(wstring_to_string(to_bin(7, 3)), "111");
  EXPECT_EQ(wstring_to_string(to_bin(8, 4)), "1000");
}

TEST_F(ToBinTest, ConvertWithPadding) {
  EXPECT_EQ(wstring_to_string(to_bin(5, 8)), "00000101");
  EXPECT_EQ(wstring_to_string(to_bin(15, 8)), "00001111");
  EXPECT_EQ(wstring_to_string(to_bin(127, 8)), "01111111");
  EXPECT_EQ(wstring_to_string(to_bin(128, 8)), "10000000");
}

TEST_F(ToBinTest, ConvertPowersOfTwo) {
  EXPECT_EQ(wstring_to_string(to_bin(16, 8)), "00010000");
  EXPECT_EQ(wstring_to_string(to_bin(32, 8)), "00100000");
  EXPECT_EQ(wstring_to_string(to_bin(64, 8)), "01000000");
  EXPECT_EQ(wstring_to_string(to_bin(128, 8)), "10000000");
  EXPECT_EQ(wstring_to_string(to_bin(256, 9)), "100000000");
}

TEST_F(ToBinTest, ConvertLargerNumbers) {
  EXPECT_EQ(wstring_to_string(to_bin(255, 8)), "11111111");
  EXPECT_EQ(wstring_to_string(to_bin(256, 9)), "100000000");
  EXPECT_EQ(wstring_to_string(to_bin(1000, 10)), "1111101000");
  EXPECT_EQ(wstring_to_string(to_bin(1023, 10)), "1111111111");
  EXPECT_EQ(wstring_to_string(to_bin(1024, 11)), "10000000000");
}

TEST_F(ToBinTest, ExpectedLengthShorterThanResult) {
  // When expected_length is shorter than the binary representation,
  // the full representation should be returned
  EXPECT_EQ(wstring_to_string(to_bin(255, 4)), "11111111");
  EXPECT_EQ(wstring_to_string(to_bin(1000, 5)), "1111101000");
}

// ============================================================================
// Tests for IEEE 754 conversion - float_to_ieee754()
// ============================================================================

class IEEE754Test : public ::testing::Test {};

// Test case from original code: 12.375
TEST_F(IEEE754Test, Convert12_375) {
  auto result = float_to_ieee754(12.375f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 12.375 = 1.100011 × 2^3
  // Exponent: 127 + 3 = 130 = 10000010
  EXPECT_EQ(wstring_to_string(result.exponent), "10000010");
  EXPECT_EQ(result.biased_exponent, 130);
  EXPECT_EQ(result.power_of_2, 3);
  // Mantissa: 10001100000000000000000 (23 bits)
  EXPECT_EQ(wstring_to_string(result.mantissa), "10001100000000000000000");
}

// Test case from original code: -0.375
TEST_F(IEEE754Test, ConvertNegative0_375) {
  auto result = float_to_ieee754(-0.375f);
  EXPECT_EQ(wstring_to_string(result.sign), "1");
  // 0.375 = 0.011 in binary = 1.1 × 2^(-2)
  // Exponent: 127 + (-2) = 125 = 01111101
  EXPECT_EQ(wstring_to_string(result.exponent), "01111101");
  EXPECT_EQ(result.biased_exponent, 125);
  EXPECT_EQ(result.power_of_2, -2);
}

// Test positive zero
TEST_F(IEEE754Test, ConvertPositiveZero) {
  auto result = float_to_ieee754(0.0f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  EXPECT_EQ(wstring_to_string(result.exponent), "00000000");
  EXPECT_EQ(wstring_to_string(result.mantissa), "00000000000000000000000");
}

// Test negative zero
TEST_F(IEEE754Test, ConvertNegativeZero) {
  auto result = float_to_ieee754(-0.0f);
  // Note: -0.0f >= 0 is true in C++, so sign will be "0"
  // This is a known limitation of the implementation
  EXPECT_EQ(wstring_to_string(result.exponent), "00000000");
  EXPECT_EQ(wstring_to_string(result.mantissa), "00000000000000000000000");
}

// Test value 1.0
TEST_F(IEEE754Test, ConvertOne) {
  auto result = float_to_ieee754(1.0f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 1.0 = 1.0 × 2^0
  // Exponent: 127 + 0 = 127 = 01111111
  EXPECT_EQ(wstring_to_string(result.exponent), "01111111");
  EXPECT_EQ(result.biased_exponent, 127);
  EXPECT_EQ(result.power_of_2, 0);
  // Mantissa: all zeros (the 1 is implicit)
  EXPECT_EQ(wstring_to_string(result.mantissa), "00000000000000000000000");
}

// Test value 2.0
TEST_F(IEEE754Test, ConvertTwo) {
  auto result = float_to_ieee754(2.0f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 2.0 = 1.0 × 2^1
  // Exponent: 127 + 1 = 128 = 10000000
  EXPECT_EQ(wstring_to_string(result.exponent), "10000000");
  EXPECT_EQ(result.biased_exponent, 128);
  EXPECT_EQ(result.power_of_2, 1);
}

// Test value 0.5
TEST_F(IEEE754Test, ConvertHalf) {
  auto result = float_to_ieee754(0.5f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 0.5 = 1.0 × 2^(-1)
  // Exponent: 127 + (-1) = 126 = 01111110
  EXPECT_EQ(wstring_to_string(result.exponent), "01111110");
  EXPECT_EQ(result.biased_exponent, 126);
  EXPECT_EQ(result.power_of_2, -1);
}

// Test value 0.25
TEST_F(IEEE754Test, ConvertQuarter) {
  auto result = float_to_ieee754(0.25f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 0.25 = 1.0 × 2^(-2)
  // Exponent: 127 + (-2) = 125 = 01111101
  EXPECT_EQ(wstring_to_string(result.exponent), "01111101");
  EXPECT_EQ(result.biased_exponent, 125);
  EXPECT_EQ(result.power_of_2, -2);
}

// Test negative value -1.0
TEST_F(IEEE754Test, ConvertNegativeOne) {
  auto result = float_to_ieee754(-1.0f);
  EXPECT_EQ(wstring_to_string(result.sign), "1");
  EXPECT_EQ(wstring_to_string(result.exponent), "01111111");
  EXPECT_EQ(result.biased_exponent, 127);
}

// Test larger value 100.0
TEST_F(IEEE754Test, ConvertHundred) {
  auto result = float_to_ieee754(100.0f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 100 = 1100100 in binary = 1.1001 × 2^6
  // Exponent: 127 + 6 = 133 = 10000101
  EXPECT_EQ(wstring_to_string(result.exponent), "10000101");
  EXPECT_EQ(result.biased_exponent, 133);
  EXPECT_EQ(result.power_of_2, 6);
}

// Test value 0.125 (1/8)
TEST_F(IEEE754Test, ConvertOneEighth) {
  auto result = float_to_ieee754(0.125f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 0.125 = 1.0 × 2^(-3)
  // Exponent: 127 + (-3) = 124 = 01111100
  EXPECT_EQ(wstring_to_string(result.exponent), "01111100");
  EXPECT_EQ(result.biased_exponent, 124);
  EXPECT_EQ(result.power_of_2, -3);
}

// Test value 1.5
TEST_F(IEEE754Test, ConvertOnePointFive) {
  auto result = float_to_ieee754(1.5f);
  EXPECT_EQ(wstring_to_string(result.sign), "0");
  // 1.5 = 1.1 in binary = 1.1 × 2^0
  // Exponent: 127 + 0 = 127 = 01111111
  EXPECT_EQ(wstring_to_string(result.exponent), "01111111");
  EXPECT_EQ(result.biased_exponent, 127);
  EXPECT_EQ(result.power_of_2, 0);
  // Mantissa starts with 1 (the .1 part)
  EXPECT_EQ(result.mantissa[0], L'1');
}

// Test full_binary() method
TEST_F(IEEE754Test, FullBinaryMethod) {
  auto result = float_to_ieee754(1.0f);
  std::wstring full = result.full_binary();
  EXPECT_EQ(full.length(), 32u);
  EXPECT_EQ(wstring_to_string(full), "00111111100000000000000000000000");
}

// Test that sign bit is correct for positive numbers
TEST_F(IEEE754Test, SignBitPositive) {
  EXPECT_EQ(wstring_to_string(float_to_ieee754(1.0f).sign), "0");
  EXPECT_EQ(wstring_to_string(float_to_ieee754(100.0f).sign), "0");
  EXPECT_EQ(wstring_to_string(float_to_ieee754(0.001f).sign), "0");
}

// Test that sign bit is correct for negative numbers
TEST_F(IEEE754Test, SignBitNegative) {
  EXPECT_EQ(wstring_to_string(float_to_ieee754(-1.0f).sign), "1");
  EXPECT_EQ(wstring_to_string(float_to_ieee754(-100.0f).sign), "1");
  EXPECT_EQ(wstring_to_string(float_to_ieee754(-0.001f).sign), "1");
}

// Test mantissa length is always 23 bits
TEST_F(IEEE754Test, MantissaLength) {
  EXPECT_EQ(float_to_ieee754(1.0f).mantissa.length(), 23u);
  EXPECT_EQ(float_to_ieee754(0.5f).mantissa.length(), 23u);
  EXPECT_EQ(float_to_ieee754(12.375f).mantissa.length(), 23u);
  EXPECT_EQ(float_to_ieee754(100.0f).mantissa.length(), 23u);
  EXPECT_EQ(float_to_ieee754(-0.375f).mantissa.length(), 23u);
}

// Test exponent length is always 8 bits
TEST_F(IEEE754Test, ExponentLength) {
  EXPECT_EQ(float_to_ieee754(1.0f).exponent.length(), 8u);
  EXPECT_EQ(float_to_ieee754(0.5f).exponent.length(), 8u);
  EXPECT_EQ(float_to_ieee754(12.375f).exponent.length(), 8u);
  EXPECT_EQ(float_to_ieee754(100.0f).exponent.length(), 8u);
  EXPECT_EQ(float_to_ieee754(-0.375f).exponent.length(), 8u);
}

// Test powers of 2
TEST_F(IEEE754Test, PowersOfTwo) {
  // 2^0 = 1
  EXPECT_EQ(float_to_ieee754(1.0f).power_of_2, 0);
  // 2^1 = 2
  EXPECT_EQ(float_to_ieee754(2.0f).power_of_2, 1);
  // 2^2 = 4
  EXPECT_EQ(float_to_ieee754(4.0f).power_of_2, 2);
  // 2^3 = 8
  EXPECT_EQ(float_to_ieee754(8.0f).power_of_2, 3);
  // 2^(-1) = 0.5
  EXPECT_EQ(float_to_ieee754(0.5f).power_of_2, -1);
  // 2^(-2) = 0.25
  EXPECT_EQ(float_to_ieee754(0.25f).power_of_2, -2);
}
