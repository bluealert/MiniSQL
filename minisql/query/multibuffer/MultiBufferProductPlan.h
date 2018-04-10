#pragma once

#include <query/materialize/TempTable.h>
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
namespace multibuffer {

class MultiBufferProductPlan : public Plan {
 public:
  MultiBufferProductPlan(const std::shared_ptr<Plan> &lhs,
                         const std::shared_ptr<Plan> &rhs,
                         storage::tx::Transaction &tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string &fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::unique_ptr<materialize::TempTable> copyRecordsFrom(
      std::shared_ptr<Plan> p) const;

 private:
  std::shared_ptr<Plan> lhs_, rhs_;
  storage::tx::Transaction &tx_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql