#pragma once

#include <memory>
#include <string>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class Scan;
class Plan;
class Expression;

class Term {
 public:
  Term(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);

  int32_t reductionFactor(const Plan& p);

  std::shared_ptr<common::Constant> equatesWithConstant(
      const std::string& fldName);

  std::string equatesWithField(const std::string& fldName);
  bool appliesTo(const storage::record::Schema& sch);
  bool isSatisfied(const Scan& s);
  std::string toString();

 private:
  std::unique_ptr<Expression> lhs_, rhs_;
};

}  // namespace query
}  // namespace minisql