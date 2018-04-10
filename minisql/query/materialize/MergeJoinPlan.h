#pragma once

#include <query/plan/Plan.h>
#include <string>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
}  // namespace storage

namespace query {
namespace materialize {

class MergeJoinPlan : public Plan {
 public:
  MergeJoinPlan(const std::shared_ptr<Plan> &p1,
                const std::shared_ptr<Plan> &p2, const std::string &fldName1,
                const std::string &fldName2, storage::tx::Transaction &tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string &fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p1_, p2_;
  std::string fldName1_, fldName2_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql