#pragma once

#include <string>

namespace minisql {
namespace common {

class Constant {
 public:
  enum class Type { INTEGER = 0, STRING = 1 };

  virtual ~Constant() = default;

  virtual Type type() const = 0;
  virtual int32_t hashCode() const = 0;
  virtual int32_t size() const = 0;
  virtual int32_t compareTo(const Constant& rhs) const = 0;
  virtual bool equals(const Constant& rhs) const = 0;
  virtual std::string toString() const = 0;
};

}  // namespace common
}  // namespace minisql