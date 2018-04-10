#pragma once

#include <exception>

namespace minisql {
namespace storage {
namespace buffer {

class BufferAbortException : public std::exception {
 public:
  const char *what() const noexcept override { return "can't pin a buffer"; }
};

}  // namespace buffer
}  // namespace storage
}  // namespace minisql