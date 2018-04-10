#pragma once

#include <storage/record/RID.h>
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
class TableInfo;
}  // namespace record

namespace index {
namespace btree {

class BTreePage {
 public:
  BTreePage(const std::shared_ptr<file::Block> &currBlk,
            const std::shared_ptr<record::TableInfo> &ti, tx::Transaction &tx);

  int32_t findSlotBefore(const std::shared_ptr<common::Constant> &searchKey);
  void close();
  bool isFull();
  std::shared_ptr<file::Block> split(int32_t splitPos, int32_t flag);
  std::shared_ptr<common::Constant> getDataVal(int32_t slot);
  int32_t getFlag();
  void setFlag(int32_t val);
  int32_t getChildNum(int32_t slot);
  void insertDir(int32_t slot, const std::shared_ptr<common::Constant> &val,
                 int32_t blkNum);
  std::unique_ptr<record::RID> getDataRid(int32_t slot);
  void insertLeaf(int32_t slot, const std::shared_ptr<common::Constant> &val,
                  const std::shared_ptr<record::RID> &rid);
  void del(int32_t slot);
  int32_t getNumRecs();

 private:
  std::shared_ptr<file::Block> appendNew(int32_t flag);
  int32_t getInt32(int32_t slot, const std::string &fldName);
  std::string getString(int32_t slot, const std::string &fldName);
  std::shared_ptr<common::Constant> getVal(int32_t slot,
                                           const std::string &fldName);
  void setInt32(int32_t slot, const std::string &fldName, int32_t val);
  void setString(int32_t slot, const std::string &fldName,
                 const std::string &val);
  void setVal(int32_t slot, const std::string &fldName,
              const std::shared_ptr<common::Constant> &val);
  void setNumRecs(int32_t n);
  void insert(int32_t slot);
  void copyRecord(int32_t from, int32_t to);
  void transferRecs(int32_t slot, BTreePage &dest);
  int32_t fldPos(int32_t slot, const std::string &fldName);
  int32_t slotPos(int32_t slot);

 private:
  int32_t slotSize_;
  std::shared_ptr<file::Block> currBlk_;
  std::shared_ptr<record::TableInfo> ti_;
  tx::Transaction &tx_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql