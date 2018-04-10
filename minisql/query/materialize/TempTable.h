#pragma once

#include <query/scan/UpdateScan.h>
#include <memory>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class Schema;
class TableInfo;
}  // namespace record
}  // namespace storage

namespace query {
namespace materialize {

class TempTable {
 public:
  explicit TempTable(const std::shared_ptr<storage::record::Schema> &sch,
                     storage::tx::Transaction &tx);

  std::unique_ptr<query::UpdateScan> open();
  std::shared_ptr<storage::record::TableInfo> getTableInfo();

 private:
  storage::tx::Transaction &tx_;
  std::shared_ptr<storage::record::TableInfo> ti_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql