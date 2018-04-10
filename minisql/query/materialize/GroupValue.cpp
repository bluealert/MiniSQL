#include <common/Constant.h>
#include <query/materialize/GroupValue.h>
#include <query/scan/Scan.h>

namespace minisql {
namespace query {
namespace materialize {

GroupValue::GroupValue(Scan &s, const std::vector<std::string> &fields) {
  for (auto &f : fields) {
    vals_.insert(std::make_pair(f, s.getVal(f)));
  }
}

std::shared_ptr<common::Constant> GroupValue::getVal(
    const std::string &fldName) {
  return vals_[fldName];
}

bool GroupValue::equals(const GroupValue &cgv) {
  auto gv = const_cast<GroupValue &>(cgv);
  for (auto &kv : vals_) {
    auto v1 = kv.second;
    auto v2 = gv.getVal(kv.first);
    if (v1->equals(*v2)) {
      return false;
    }
  }
  return true;
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql