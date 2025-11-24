#ifndef MIPS_H
#define MIPS_H

#include <string>
#include <vector>

namespace mips {

// Structure representing a single instruction type's execution metrics
struct InstructionMetrics {
  std::wstring name;
  long long count = 0;
  long long time_in_ns = 0;
  long long total_time_ns = 0;  // count * time_in_ns
};

// MIPS Calculator class
// Calculates Million Instructions Per Second based on instruction execution
// metrics
class MIPSCalculator {
 public:
  MIPSCalculator() : total_instructions_(0), total_time_ns_(0) {}

  // Add instruction execution metrics
  void add(const std::wstring& name, long long count, long long time_in_ns) {
    InstructionMetrics metrics;
    metrics.name = name;
    metrics.count = count;
    metrics.time_in_ns = time_in_ns;
    metrics.total_time_ns = count * time_in_ns;

    instructions_.push_back(metrics);
    total_instructions_ += count;
    total_time_ns_ += metrics.total_time_ns;
  }

  // Calculate MIPS rating
  // MIPS = (Total instructions / Total time in seconds) / 10^6
  double calculate_mips() const {
    if (total_time_ns_ == 0) {
      return 0.0;
    }
    double total_time_sec = total_time_ns_ * 1e-9;
    return (static_cast<double>(total_instructions_) / total_time_sec) / 1e6;
  }

  // Get total number of instructions
  long long get_total_instructions() const { return total_instructions_; }

  // Get total execution time in nanoseconds
  long long get_total_time_ns() const { return total_time_ns_; }

  // Get total execution time in seconds
  double get_total_time_sec() const { return total_time_ns_ * 1e-9; }

  // Get all instruction metrics
  const std::vector<InstructionMetrics>& get_instructions() const {
    return instructions_;
  }

  // Reset calculator
  void reset() {
    instructions_.clear();
    total_instructions_ = 0;
    total_time_ns_ = 0;
  }

 private:
  std::vector<InstructionMetrics> instructions_;
  long long total_instructions_;
  long long total_time_ns_;
};

}  // namespace mips

#endif  // MIPS_H
