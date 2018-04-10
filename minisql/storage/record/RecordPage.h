#pragma once

#include <memory>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace file {
class Block;
}

namespace record {

class TableInfo;

class RecordPage {
 public:
  static constexpr int32_t EMPTY{0}, INUSE{1};

  RecordPage(const std::shared_ptr<file::Block> &blk,
             const std::shared_ptr<TableInfo> &ti, tx::Transaction &tx);

  bool next();
  int32_t getInt32(const std::string &fldName);
  std::string getString(const std::string &fldName);

  bool insert();
  void setInt32(const std::string &fldName, int32_t val);
  void setString(const std::string &fldName, const std::string &val);

  void del();

  void moveToSlot(int32_t slot);
  int32_t currentSlot() const;

  void close();

 private:
  inline int32_t currentPos() const;
  inline int32_t fieldPos(const std::string &fldName) const;
  inline bool isValidSlot() const;
  inline bool searchFor(int32_t flag);

 private:
  int32_t slotSize_;
  int32_t currSlot_;
  tx::Transaction &tx_;
  std::shared_ptr<TableInfo> ti_;
  std::shared_ptr<file::Block> blk_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql