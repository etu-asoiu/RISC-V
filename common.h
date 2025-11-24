#ifndef COMMON_H
#define COMMON_H

#include <fcntl.h>
#include <cstdio>
#include <cwchar>
#include <fstream>
#include <iostream>

// Инициализация кодировки UTF-16 для Windows консоли
inline void initWindowsEncoding() {
#if WIN32 || WIN64
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stdin), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
#endif
}

#endif // COMMON_H
