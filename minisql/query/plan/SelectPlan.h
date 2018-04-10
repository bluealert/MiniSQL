#pragma once

#include <query/Predicate.h>
#include <query/plan/Plan.h>

namespace minisql {
namespace query {

class Predicate;

class SelectPlan : public Plan {
 public:
  SelectPlan(const std::shared_ptr<Plan>& p,
             const std::shared_ptr<Predicate>& pred);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p_;
  std::shared_ptr<Predicate> pred_;
};

}  // namespace query
}  // namespace minisql