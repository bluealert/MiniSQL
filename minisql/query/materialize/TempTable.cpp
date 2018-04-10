#include <query/materialize/TempTable.h>
#include <query/scan/TableScan.h>
#include <storage/record/TableInfo.h>
#include <mutex>

namespace minisql {
namespace query {
namespace materialize {

using namespace storage;

static std::string nextTableName() {
  static int32_t nextTableNum = 0;
  static std::mutex mutex_;
  std::lock_guard<std::mutex> lk(mutex_);
  nextTableNum++;
  return "temp" + std::to_string(nextTableNum);
}

TempTable::TempTable(const std::shared_ptr<record::Schema> &sch,
                     tx::Transaction &tx)
    : tx_(tx) {
  auto tblName = nextTableName();
  ti_ = std::make_shared<record::TableInfo>(tblName, sch);
}

std::unique_ptr<query::UpdateScan> TempTable::open() {
  return std::make_unique<query::TableScan>(ti_, tx_);
}

std::shared_ptr<record::TableInfo> TempTable::getTableInfo() { return ti_; }

}  // namespace materialize
}  // namespace query
}  // namespace minisql