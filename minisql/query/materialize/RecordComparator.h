#pragma once

#include <query/scan/Scan.h>
#include <string>
#include <vector>

namespace minisql {
namespace query {
namespace materialize {

class RecordComparator {
 public:
  explicit RecordComparator(const std::vector<std::string> &fields);

  int32_t compare(Scan &s1, Scan &s2);

 private:
  std::vector<std::string> fields_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql