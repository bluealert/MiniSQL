#pragma once

#include <query/plan/Plan.h>

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
class Scan;

namespace materialize {

class MaterializePlan : public Plan {
 public:
  MaterializePlan(const std::shared_ptr<Plan> &srcPlan,
                  storage::tx::Transaction &tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string &fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> srcPlan_;
  storage::tx::Transaction &tx_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql