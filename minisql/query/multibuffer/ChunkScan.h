#pragma once

#include <query/scan/Scan.h>
#include <vector>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class Schema;
class RecordPage;
class TableInfo;
}  // namespace record
}  // namespace storage

namespace query {
namespace multibuffer {

class ChunkScan : public query::Scan {
 public:
  ChunkScan(const std::shared_ptr<storage::record::TableInfo> &ti,
            int32_t startBlkNum, int32_t endBlkNum,
            storage::tx::Transaction &tx);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string &fldName) const override;
  int32_t getInt32(const std::string &fldName) const override;
  std::string getString(const std::string &fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string &fldName) const override;

  void close() override;

 private:
  void moveToBlock(int32_t blkNum);

 private:
  std::vector<std::shared_ptr<storage::record::RecordPage>> pages_;
  int32_t startBlkNum_, endBlkNum_, curr_;
  std::shared_ptr<storage::record::Schema> sch_;
  std::shared_ptr<storage::record::RecordPage> rp_;
};

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql