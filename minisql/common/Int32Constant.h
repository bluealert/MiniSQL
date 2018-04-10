#pragma once

#include <common/Constant.h>

namespace minisql {
namespace common {

class Int32Constant : public Constant {
 public:
  explicit Int32Constant(int32_t val);

  Type type() const override;
  int32_t hashCode() const override;
  int32_t size() const override;
  int32_t compareTo(const Constant& rhs) const override;
  bool equals(const Constant& rhs) const override;
  std::string toString() const override;
  int32_t val() const;

 private:
  int32_t val_;
};

}  // namespace common
}  // namespace minisql