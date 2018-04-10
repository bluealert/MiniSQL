#pragma once

#include <cstdint>

namespace minisql {
namespace query {
namespace multibuffer {

class BufferNeeds {
 public:
  static int32_t bestFactor(int32_t size);
};

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql