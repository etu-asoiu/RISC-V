#include "include/mips.h"
#include <fcntl.h>
#include <cstdio>
#include <cwchar>
#include <iostream>

using namespace std;

int main() {
  // Задаём кодировку UTF-16 для всего вывода в программе
  // Все символы и строки будут wchar_t
#if WIN32 || WIN64
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stdin), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
#endif
  MIPS mips;
  mips.add(L"Сложение", 23500, 90);
  mips.add(L"Умножение", 30900, 160);
  mips.add(L"Ветвление/прыжок", 15200, 250);
  mips.add(L"Другое", 8600, 60);
  mips.calc();
  return 0;
}
