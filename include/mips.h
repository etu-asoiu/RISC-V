#ifndef MIPS_H
#define MIPS_H

#include <string>

using namespace std;

// MIPS calculation structure
// Calculates MIPS (Million Instructions Per Second) based on instruction counts and execution times
struct MIPS {
  long long total = 0;           // Total number of instructions
  long long total_time_in_ns = 0; // Total execution time in nanoseconds

  // Add instruction type with count and execution time
  void add(const wstring &name, const int num, const int time_in_ns);

  // Calculate and display MIPS value
  void calc() const;

  // Calculate MIPS value without displaying (for testing)
  double get_mips() const;
};

#endif // MIPS_H
