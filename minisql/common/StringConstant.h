#pragma once

#include <common/Constant.h>
#include <string>

namespace minisql {
namespace common {

class StringConstant : public Constant {
 public:
  explicit StringConstant(std::string val);

  Type type() const override;
  int32_t hashCode() const override;
  int32_t size() const override;
  int32_t compareTo(const Constant& rhs) const override;
  bool equals(const Constant& rhs) const override;
  std::string toString() const override;
  std::string val() const;

 private:
  mutable int32_t hash_;
  std::string val_;
};

}  // namespace common
}  // namespace minisql