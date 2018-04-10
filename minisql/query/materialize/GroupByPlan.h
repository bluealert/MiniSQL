#pragma once

#include <query/plan/Plan.h>
#include <vector>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class Schema;
}
}  // namespace storage

namespace query {
namespace materialize {

class AggregationFn;

class GroupByPlan : public Plan {
 public:
  GroupByPlan(const std::shared_ptr<Plan> &p,
              std::vector<std::string> groupFields,
              std::vector<std::shared_ptr<AggregationFn>> aggFns,
              storage::tx::Transaction &tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string &fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p_;
  std::vector<std::string> groupFields_;
  std::vector<std::shared_ptr<AggregationFn>> aggFns_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql