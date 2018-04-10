#pragma once

#include <query/plan/Plan.h>
#include <storage/record/Schema.h>
#include <string>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace metadata {
class IndexInfo;
}
}  // namespace storage

namespace query {

class IndexJoinPlan : public Plan {
 public:
  IndexJoinPlan(const std::shared_ptr<Plan>& p1,
                const std::shared_ptr<Plan>& p2,
                storage::metadata::IndexInfo& ii, std::string joinField,
                storage::tx::Transaction& tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::string joinField_;
  std::shared_ptr<Plan> p1_, p2_;
  storage::metadata::IndexInfo& ii_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace query
}  // namespace minisql