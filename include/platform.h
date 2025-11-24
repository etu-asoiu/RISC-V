#ifndef PLATFORM_H
#define PLATFORM_H

// Общий код инициализации платформы
// Common platform initialization code

#if defined(WIN32) || defined(WIN64)
#include <fcntl.h>
#include <io.h>
#include <cstdio>
#endif

namespace platform {

// Инициализация консоли для вывода UTF-16 (Windows)
// Initialize console for UTF-16 output (Windows)
inline void init_utf16_console() {
#if defined(WIN32) || defined(WIN64)
  _setmode(_fileno(stdout), _O_U16TEXT);
  _setmode(_fileno(stdin), _O_U16TEXT);
  _setmode(_fileno(stderr), _O_U16TEXT);
#endif
  // На других платформах ничего не делаем
  // No action needed on other platforms
}

}  // namespace platform

#endif  // PLATFORM_H
