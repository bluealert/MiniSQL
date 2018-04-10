#pragma once

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
class TableInfo;
}

namespace index {
namespace btree {

class DirEntry;
class BTreePage;

class BTreeDir {
 public:
  BTreeDir(const std::shared_ptr<file::Block>& blk,
           const std::shared_ptr<record::TableInfo>& ti, tx::Transaction& tx);

  void close();
  int32_t search(const std::shared_ptr<common::Constant>& searchKey);
  void makeNewRoot(const std::shared_ptr<DirEntry>& e);
  std::shared_ptr<DirEntry> insert(const std::shared_ptr<DirEntry>& e);

 private:
  std::shared_ptr<DirEntry> insertEntry(const std::shared_ptr<DirEntry>& e);

  std::shared_ptr<file::Block> findChildBlock(
      const std::shared_ptr<common::Constant>& searchKey);

 private:
  std::string fileName_;
  std::shared_ptr<record::TableInfo> ti_;
  tx::Transaction& tx_;
  std::shared_ptr<BTreePage> contents_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql