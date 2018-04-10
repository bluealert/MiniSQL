#pragma once

#include <memory>

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
class RID;
class TableInfo;
}  // namespace record

namespace index {
namespace btree {

class BTreePage;
class DirEntry;

class BTreeLeaf {
 public:
  BTreeLeaf(const std::shared_ptr<file::Block>& blk,
            const std::shared_ptr<record::TableInfo>& ti,
            const std::shared_ptr<common::Constant>& searchKey,
            tx::Transaction& tx);

  void close();
  bool next();
  std::unique_ptr<record::RID> getDataRid();
  void del(const std::shared_ptr<record::RID>& dataRid);

  std::shared_ptr<DirEntry> insert(const std::shared_ptr<record::RID>& dataRid);

 private:
  bool tryOverflow();

 private:
  int32_t currSlot_;
  tx::Transaction& tx_;
  std::shared_ptr<BTreePage> contents_;
  std::shared_ptr<record::TableInfo> ti_;
  std::shared_ptr<common::Constant> searchKey_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql