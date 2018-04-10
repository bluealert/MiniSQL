#pragma once

#include <storage/record/RID.h>
#include <storage/record/RecordPage.h>
#include <string>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace record {

class TableInfo;

class RecordFile {
 public:
  RecordFile(const std::shared_ptr<TableInfo> &ti, tx::Transaction &tx);
  virtual ~RecordFile();

  void beforeFirst();
  bool next();
  int32_t getInt32(const std::string &fldName) const;
  std::string getString(const std::string &fldName) const;

  void insert();
  void setInt32(const std::string &fldName, int32_t val);
  void setString(const std::string &fldName, const std::string &val);

  void del();

  void close();

  std::unique_ptr<RID> currentRid() const;
  void moveToRid(const RID &rid);

 private:
  void moveToBlock(int32_t b);
  bool atLastBlock();
  void appendBlock();

 private:
  std::shared_ptr<TableInfo> ti_;
  tx::Transaction &tx_;
  int32_t currBlkNum_;
  std::string fileName_;
  std::unique_ptr<RecordPage> rp_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql