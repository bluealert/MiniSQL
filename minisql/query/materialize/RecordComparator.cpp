#include <common/Constant.h>
#include <query/materialize/RecordComparator.h>

namespace minisql {
namespace query {
namespace materialize {

RecordComparator::RecordComparator(const std::vector<std::string> &fields)
    : fields_(fields) {}

int32_t RecordComparator::compare(Scan &s1, Scan &s2) {
  for (auto &fldName : fields_) {
    auto val1 = s1.getVal(fldName);
    auto val2 = s2.getVal(fldName);
    auto result = val1->compareTo(*val2);
    if (result != 0) {
      return result;
    }
  }
  return 0;
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql