#include "include/binary32.h"
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
  show(12.375);
  show(3.14159274101257324);
  show(-0.375);
  return 0;
}
