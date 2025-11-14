#include "../include/mips.h"
#include <iostream>

using namespace std;

void MIPS::add(const wstring &name, const int num, const int time_in_ns) {
  long long all = static_cast<long long>(num) * time_in_ns;  // Выполнение всех инструкций в ns
  wcout << name << " " << num << L" инструкций x " <<
        time_in_ns << " ns = " << all << endl;
  total += num; // Складываем количество выполненных инструкций вместе
  total_time_in_ns += all;  // Суммарное время в наносекунда
}

void MIPS::calc() const {
  // MIPS = (Общее количество инструкций / Общее время выполнения) × 10^6
  const double total_time_is_sec = total_time_in_ns * 1e-9; // Переводим в секунды
  wcout << "Total time: " << total_time_in_ns << " ns = " << total_time_is_sec << " sec" << endl;
  wcout << "MIPS = " << (total / total_time_is_sec / 1e6) << endl;
}

double MIPS::get_mips() const {
  if (total_time_in_ns == 0) {
    return 0.0;
  }
  const double total_time_is_sec = total_time_in_ns * 1e-9;
  return total / total_time_is_sec / 1e6;
}
