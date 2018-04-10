#pragma once

#include <storage/index/Index.h>
#include <storage/metadata/StatInfo.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace index {
class Index;
}

namespace metadata {

class IndexInfo {
 public:
  IndexInfo(const std::string &idxName, const std::string &tblName,
            const std::string &fldName, tx::Transaction &tx);

  std::unique_ptr<index::Index> open();
  int32_t blocksAccessed() const;
  int32_t recordsOutput() const;
  int32_t distinctValues(const std::string &fldName) const;

 private:
  std::shared_ptr<record::Schema> schema() const;

 private:
  std::string idxName_, fldName_;
  tx::Transaction &tx_;
  std::unique_ptr<record::TableInfo> ti_;
  std::unique_ptr<StatInfo> si_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql