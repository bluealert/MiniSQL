#pragma once

#include <memory>

namespace minisql {
namespace common {

template <typename T>
class Iterator {
 public:
  virtual ~Iterator() = default;

  virtual bool hasNext() const = 0;
  virtual std::unique_ptr<T> next() = 0;
};

}  // namespace common
}  // namespace minisql