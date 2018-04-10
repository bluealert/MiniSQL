#pragma once

#include <exception>

namespace minisql {
namespace storage {
namespace tx {
namespace concurrency {

class LockAbortException : public std::exception {
 public:
  const char *what() const noexcept override { return "can't get tx lock"; }
};

}  // namespace concurrency
}  // namespace tx
}  // namespace storage
}  // namespace minisql