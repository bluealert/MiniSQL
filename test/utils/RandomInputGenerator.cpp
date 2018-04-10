#include <test/utils/RandomInputGenerator.h>

utils::RandomInputGenerator::RandomInputGenerator() : rng_(0) {}

bool utils::RandomInputGenerator::yes() {
  std::uniform_int_distribution<char> uid(0, 2);
  return uid(rng_) == 1;
}

int32_t utils::RandomInputGenerator::number(int32_t max) {
  std::uniform_int_distribution<int32_t> uid(0, max);
  return uid(rng_);
}

std::string utils::RandomInputGenerator::string(uint8_t max) {
  std::string result;

  auto len = number(max);
  std::uniform_int_distribution<char> uid(0, 25);
  for (int i = 0; i < len; ++i) {
    char c = uid(rng_);
    result += (c + 'a');
  }

  return result;
}