#pragma once

#include <memory>
#include <vector>

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

class Term;
class Scan;
class Plan;

class Predicate {
 public:
  Predicate() = default;
  explicit Predicate(std::unique_ptr<Term> t);

  void conjoinWith(const Predicate& pred);
  bool isSatisfied(const Scan& s) const;
  int32_t reductionFactor(const Plan& p) const;

  std::shared_ptr<Predicate> selectPred(
      const storage::record::Schema& sch) const;

  std::shared_ptr<Predicate> joinPred(
      const storage::record::Schema& sch1,
      const storage::record::Schema& sch2) const;

  std::shared_ptr<common::Constant> equatesWithConstant(
      const std::string& fldName) const;

  std::string equatesWithField(const std::string& fldName) const;

  bool empty() const;
  std::string toString() const;

 private:
  std::vector<std::shared_ptr<Term>> terms_;
};

}  // namespace query
}  // namespace minisql