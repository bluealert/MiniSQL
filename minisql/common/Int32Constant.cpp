#include <common/Int32Constant.h>
#include <sstream>

namespace minisql {
namespace common {

Int32Constant::Int32Constant(int32_t val) : val_(val) {}

Constant::Type Int32Constant::type() const { return Type::INTEGER; }

int32_t Int32Constant::hashCode() const { return val_; }

int32_t Int32Constant::size() const { return sizeof(int32_t); }

int32_t Int32Constant::val() const { return val_; }

bool Int32Constant::equals(const Constant& rhs) const {
  auto p = dynamic_cast<const Int32Constant*>(&rhs);
  return p != nullptr && val_ == p->val_;
}

std::string Int32Constant::toString() const { return std::to_string(val_); }

int32_t Int32Constant::compareTo(const Constant& rhs) const {
  auto p = dynamic_cast<const Int32Constant*>(&rhs);
  if (p == nullptr) {
    return 1;
  }
  if (val_ == p->val_) {
    return 0;
  } else if (val_ > p->val_) {
    return 1;
  } else {
    return -1;
  }
}

}  // namespace common
}  // namespace minisql