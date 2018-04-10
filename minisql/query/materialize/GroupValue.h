#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace minisql {

namespace common {
class Constant;
}

namespace query {
class Scan;

namespace materialize {

class GroupValue {
 public:
  GroupValue(Scan& s, const std::vector<std::string>& fields);

  std::shared_ptr<common::Constant> getVal(const std::string& fldName);

  bool equals(const GroupValue& gv);

 private:
  std::unordered_map<std::string, std::shared_ptr<common::Constant>> vals_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql