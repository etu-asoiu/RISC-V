#include "../include/binary32.h"
#include <cmath>
#include <iostream>

using namespace std;

// В двоичную систему счисления
wstring to_bin(int x, const int expected_length) {
  if (x == 0) {
    wstring s;
    while (s.length() < expected_length) {
      s += L"0";
    }
    return s;
  }

  wstring s;
  while (x > 0) {
    s = (x % 2 == 0 ? L"0" : L"1") + s;
    x /= 2;
  }
  // Дописываем спереди нули до нужной длины
  while (s.length() < expected_length) {
    s = L"0" + s;
  }
  return s;
}

IEEE754Components get_ieee754_components(const float value) {
  IEEE754Components result;

  // Знак числа
  result.sign = (value >= 0) ? L"0" : L"1";
  float abs_value = abs(value);

  int int_part = static_cast<int>(abs_value);
  // Переводим целую часть в двоичный вид
  wstring s = to_bin(int_part, 1);

  float fraction = abs_value - int_part;

  // Переводим дробную часть в двоичный вид
  wstring frac_bin;    // Двоичное представление дроби
  int first_one = -1;  // Нет единичек
  for (int i = 0; i < first_one + 24; i++) {
    fraction *= 2;
    if (fraction >= 1.0f) {
      frac_bin += L"1";
      if (first_one == -1) {
        first_one = i;  // Запоминаем позицию первой единички
      }
      fraction -= 1.0f;
    } else {
      frac_bin += L"0";
    }
  }

  int power_of_2 = s.length() - 1;
  if (int_part == 0) {  // Обработка дробных чисел
    power_of_2 = -first_one - 1;
  }
  result.power_of_2 = power_of_2;

  const int exponenta = 127 + power_of_2;
  result.exponent = to_bin(exponenta, 8);

  result.mantissa = (s + frac_bin).substr(1, 23);
  if (int_part == 0) {
    result.mantissa = frac_bin.substr(first_one + 1, 23);
  }

  return result;
}

void show(const float value) {
  // Знак числа
  const wstring sign = (value >= 0) ? L"0" : L"1";
  float abs_value = abs(value);
  wcout << L"Число: " << value << L" Бит знака: " << sign
        << L" Абсолютное значение " << abs_value << "\n";
  int int_part = static_cast<int>(abs_value);
  // Переводим целую часть в двоичный вид
  wstring s = to_bin(int_part, 1);
  wcout << L"Целая часть числа: " << int_part << L" = " << s << "\n";
  float fraction = abs_value - int_part;
  wcout << L"Дробная часть числа: " << fraction << "\n";
  // Переводим дробную часть в двоичный вид
  wstring frac_bin;    // Двоичное представление дроби
  int first_one = -1;  // Нет единичек
  for (int i = 0; i < first_one + 24; i++) {
    fraction *= 2;
    if (fraction >= 1.0f) {
      frac_bin += L"1";
      if (first_one == -1) {
        first_one = i;  // Запоминаем позицию первой единички
      }
      fraction -= 1.0f;
    } else {
      frac_bin += L"0";
    }
  }
  wcout << L"Число: " << s << "." << frac_bin << " first_one=" << first_one
        << L"\n";
  int power_of_2 = s.length() - 1;
  if (int_part == 0) {  // Обработка дробных чисел
    power_of_2 = -first_one - 1;
  }
  const int exponenta = 127 + power_of_2;
  wstring exp_str = to_bin(exponenta, 8);
  wcout << L"Степень 2: " << power_of_2 << L" exponenta " << exp_str << "\n";

  wstring mantissa = (s + frac_bin).substr(1, 23);
  if (int_part == 0) {
    mantissa = frac_bin.substr(first_one + 1, 23);
  }
  wcout << sign << " " << exp_str << " " << mantissa << "\n\n";
}
