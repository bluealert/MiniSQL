#pragma once

#include <memory>
#include <unordered_map>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace record {
class TableInfo;
}

namespace metadata {

class IndexInfo;
class TableMgr;

class IndexMgr {
 public:
  IndexMgr(bool isNew, const std::shared_ptr<TableMgr> &tblMgr,
           tx::Transaction &tx);

  void createIndex(const std::string &idxName, const std::string &tblName,
                   const std::string &fldName, tx::Transaction &tx);

  std::unordered_map<std::string, std::shared_ptr<IndexInfo>> getIndexInfo(
      const std::string &tblName, tx::Transaction &tx);

 private:
  std::shared_ptr<record::TableInfo> ti_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql