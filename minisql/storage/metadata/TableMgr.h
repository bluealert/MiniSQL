#pragma once

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

namespace metadata {

class TableMgr {
 public:
  static const int32_t MAX_NAME{16};

  TableMgr(bool isNew, tx::Transaction &tx);
  void createTable(const std::string &tblName,
                   const std::shared_ptr<record::Schema> &sch,
                   tx::Transaction &tx);
  std::shared_ptr<record::TableInfo> getTableInfo(const std::string &tblName,
                                                  tx::Transaction &tx);

 private:
  int32_t getRecordLengthByTblName(const std::string &tblName,
                                   tx::Transaction &tx);

 private:
  std::shared_ptr<record::TableInfo> tblCatInfo_, fldCatInfo_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql