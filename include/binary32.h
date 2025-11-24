#ifndef BINARY32_H
#define BINARY32_H

#include <cmath>
#include <string>

namespace binary32 {

// Converts an integer to binary string representation with expected length
// Pads with leading zeros if necessary
inline std::wstring to_bin(int x, const int expected_length) {
  std::wstring s;
  if (x == 0) {
    s = L"0";
  } else {
    while (x > 0) {
      s = (x % 2 == 0 ? L"0" : L"1") + s;
      x /= 2;
    }
  }
  // Pad with leading zeros to reach expected length
  while (static_cast<int>(s.length()) < expected_length) {
    s = L"0" + s;
  }
  return s;
}

// Structure to hold IEEE 754 single precision float representation
struct IEEE754Result {
  std::wstring sign;      // Sign bit (1 bit)
  std::wstring exponent;  // Exponent (8 bits)
  std::wstring mantissa;  // Mantissa (23 bits)
  int power_of_2;         // Power of 2 (unbiased exponent)
  int biased_exponent;    // Biased exponent (power + 127)

  // Returns full 32-bit binary representation
  std::wstring full_binary() const { return sign + exponent + mantissa; }
};

// Converts a float to IEEE 754 single precision representation
inline IEEE754Result float_to_ieee754(float value) {
  IEEE754Result result;

  // Sign bit
  result.sign = (value >= 0) ? L"0" : L"1";
  float abs_value = std::abs(value);

  // Handle special case of zero
  if (abs_value == 0.0f) {
    result.exponent = L"00000000";
    result.mantissa = L"00000000000000000000000";
    result.power_of_2 = -127;
    result.biased_exponent = 0;
    return result;
  }

  // Integer part
  int int_part = static_cast<int>(abs_value);
  std::wstring int_bin = to_bin(int_part, 1);

  // Fractional part conversion
  float fraction = abs_value - int_part;
  std::wstring frac_bin;
  int first_one = -1;

  // Convert fractional part to binary (up to 24 bits after first 1)
  for (int i = 0; i < (first_one == -1 ? 150 : first_one + 24); i++) {
    fraction *= 2;
    if (fraction >= 1.0f) {
      frac_bin += L"1";
      if (first_one == -1) {
        first_one = i;
      }
      fraction -= 1.0f;
    } else {
      frac_bin += L"0";
    }
    if (first_one != -1 && static_cast<int>(frac_bin.length()) >= first_one + 24)
      break;
  }

  // Calculate power of 2
  result.power_of_2 = static_cast<int>(int_bin.length()) - 1;
  if (int_part == 0) {
    result.power_of_2 = -first_one - 1;
  }

  // Calculate biased exponent
  result.biased_exponent = 127 + result.power_of_2;
  result.exponent = to_bin(result.biased_exponent, 8);

  // Extract mantissa (23 bits after the implicit leading 1)
  std::wstring full_significand = int_bin + frac_bin;
  if (int_part == 0) {
    result.mantissa = frac_bin.substr(first_one + 1, 23);
  } else {
    result.mantissa = full_significand.substr(1, 23);
  }

  // Pad mantissa to 23 bits if needed
  while (result.mantissa.length() < 23) {
    result.mantissa += L"0";
  }

  return result;
}

// Helper function to convert wstring to string for testing output
inline std::string wstring_to_string(const std::wstring& ws) {
  return std::string(ws.begin(), ws.end());
}

}  // namespace binary32

#endif  // BINARY32_H
