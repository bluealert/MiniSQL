#pragma once

#include <query/plan/Plan.h>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class TableInfo;
}
namespace metadata {
class StatInfo;
}
}  // namespace storage

namespace query {

class TablePlan : public Plan {
 public:
  TablePlan(const std::string& tblName, storage::tx::Transaction& tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  storage::tx::Transaction& tx_;
  std::shared_ptr<storage::record::TableInfo> ti_;
  std::shared_ptr<storage::metadata::StatInfo> si_;
};

}  // namespace query
}  // namespace minisql