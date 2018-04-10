#pragma once

#include <query/scan/Scan.h>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class TableInfo;
}
}  // namespace storage

namespace query {
namespace multibuffer {

class MultiBufferProductScan : public Scan {
 public:
  MultiBufferProductScan(std::unique_ptr<Scan> lhsScan,
                         const std::shared_ptr<storage::record::TableInfo> &ti,
                         storage::tx::Transaction &tx);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string &fldName) const override;
  int32_t getInt32(const std::string &fldName) const override;
  std::string getString(const std::string &fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string &fldName) const override;

  void close() override;

  bool useNextChunk();

 private:
  std::unique_ptr<query::Scan> lhsScan_, rhsScan_, prodScan_;
  std::shared_ptr<storage::record::TableInfo> ti_;
  storage::tx::Transaction &tx_;
  int32_t chunkSize_, nextBlkNum_, fileSize_;
};

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql