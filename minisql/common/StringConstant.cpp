#include <common/StringConstant.h>

namespace minisql {
namespace common {

StringConstant::StringConstant(std::string val)
    : hash_(0), val_(std::move(val)) {}

Constant::Type StringConstant::type() const { return Type::STRING; }

int32_t StringConstant::hashCode() const {
  int32_t h = hash_;
  if (h == 0 && val_.length() > 0) {
    for (auto c : val_) {
      h = 31 * h + c;
    }
    hash_ = h;
  }
  return h;
}

int32_t StringConstant::size() const { return sizeof(int32_t) + val_.size(); }

std::string StringConstant::val() const { return val_; }

bool StringConstant::equals(const Constant& rhs) const {
  auto p = dynamic_cast<const StringConstant*>(&rhs);
  return p != nullptr && val_ == p->val_;
}

std::string StringConstant::toString() const { return val_; }

int32_t StringConstant::compareTo(const Constant& rhs) const {
  auto p = dynamic_cast<const StringConstant*>(&rhs);
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