#include <iostream>
#include <iostream>
#include <fcntl.h>
#include <cstdio>
#include <cwchar>
#include <fstream>

using namespace std;

// Расчёт MIPS:
// Для расчёта MIPS, нужно определить общее время выполнения всех инструкций,
// а затем использовать это значение для расчета MIPS.
// Общее время выполнения =
//   (Количество инструкций × Время выполнения одной инструкции)
// для каждого типа инструкции.
struct MIPS {
  long long total = 0; // Общее количество инструкций
  long long total_time_in_ns = 0; // Общее время выполнения в ns
  void add(const wstring &name, const int num, const int time_in_ns) {
    long long all = num * time_in_ns;  // Выполнение всех инструкций в ns
    wcout << name << " " << num << L" инструкций x " <<
          time_in_ns << " ns = " << all << endl;
    total += num; // Складываем количество выполненных инструкций вместе
    total_time_in_ns += all;  // Суммарное время в наносекунда
  }
  void calc() const {
    // MIPS = (Общее количество инструкций / Общее время выполнения) × 10^6
    const double total_time_is_sec = total_time_in_ns * 1e-9; // Переводим в секунды
    wcout << "Total time: " << total_time_in_ns << " ns = " << total_time_is_sec << " sec" << endl;
    wcout << "MIPS = " << (total / total_time_is_sec / 1e6) << endl;
  }
};

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
