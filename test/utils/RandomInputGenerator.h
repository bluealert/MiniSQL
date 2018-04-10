#pragma once

#include <ctime>
#include <random>
#include <string>

namespace utils {

class RandomInputGenerator {
 public:
  RandomInputGenerator();

  bool yes();
  int32_t number(int32_t max);
  std::string string(uint8_t max);

 private:
  std::mt19937 rng_;
};

}  // namespace utils