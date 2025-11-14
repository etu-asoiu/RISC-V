#ifndef BINARY32_H
#define BINARY32_H

#include <string>

using namespace std;

// Convert integer to binary string with expected length
wstring to_bin(int x, const int expected_length);

// Display IEEE 754 single precision floating point representation
void show(const float value);

// Get IEEE 754 components without printing (for testing)
struct IEEE754Components {
  wstring sign;
  wstring exponent;
  wstring mantissa;
  int power_of_2;
};

IEEE754Components get_ieee754_components(const float value);

#endif // BINARY32_H
