#pragma once

#include <storage/record/RID.h>
#include <memory>
#include <string>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {

namespace tx {
class Transaction;
}
namespace file {
class Block;
}
namespace record {
class Schema;
class TableInfo;
}  // namespace record

namespace index {
namespace btree {

class BTreeLeaf;

class BTreeIndex {
 public:
  BTreeIndex(const std::string& idxName,
             const std::shared_ptr<record::Schema>& leafSch,
             tx::Transaction& tx);

  void beforeFirst(const std::shared_ptr<common::Constant>& searchKey);
  bool next();
  std::unique_ptr<record::RID> getDataRid();
  void insert(const std::shared_ptr<common::Constant>& dataVal,
              const std::shared_ptr<record::RID>& dataRid);
  void del(const std::shared_ptr<common::Constant>& dataVal,
           const std::shared_ptr<record::RID>& dataRid);
  void close();

 private:
  tx::Transaction& tx_;
  std::shared_ptr<BTreeLeaf> leaf_;
  std::shared_ptr<file::Block> rootBlk_;
  std::shared_ptr<record::TableInfo> dirTi_, leafTi_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql