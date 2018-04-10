#include <query/multibuffer/BufferNeeds.h>
#include <query/multibuffer/ChunkScan.h>
#include <query/multibuffer/MultiBufferProductScan.h>
#include <query/scan/ProductScan.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace query {
namespace multibuffer {

MultiBufferProductScan::MultiBufferProductScan(
    std::unique_ptr<Scan> lhsScan,
    const std::shared_ptr<storage::record::TableInfo> &ti,
    storage::tx::Transaction &tx)
    : lhsScan_(std::move(lhsScan)), ti_(ti), tx_(tx) {
  fileSize_ = tx_.blockCount(ti->fileName());
  chunkSize_ = BufferNeeds::bestFactor(fileSize_);
  beforeFirst();
}

void MultiBufferProductScan::beforeFirst() {
  nextBlkNum_ = 0;
  useNextChunk();
}

bool MultiBufferProductScan::next() {
  while (!prodScan_->next()) {
    if (!useNextChunk()) {
      return false;
    }
  }
  return true;
}

void MultiBufferProductScan::close() { prodScan_->close(); }

std::shared_ptr<common::Constant> MultiBufferProductScan::getVal(
    const std::string &fldName) const {
  return prodScan_->getVal(fldName);
}

int32_t MultiBufferProductScan::getInt32(const std::string &fldName) const {
  return prodScan_->getInt32(fldName);
}

std::string MultiBufferProductScan::getString(
    const std::string &fldName) const {
  return prodScan_->getString(fldName);
}

bool MultiBufferProductScan::hasField(const std::string &fldName) const {
  return prodScan_->hasField(fldName);
}

bool MultiBufferProductScan::useNextChunk() {
  if (rhsScan_ != nullptr) {
    rhsScan_->close();
  }
  if (nextBlkNum_ >= fileSize_) {
    return false;
  }
  int32_t end = nextBlkNum_ + chunkSize_ - 1;
  if (end >= fileSize_) {
    end = fileSize_ - 1;
  }
  rhsScan_ = std::make_unique<ChunkScan>(ti_, nextBlkNum_, end, tx_);
  lhsScan_->beforeFirst();
  prodScan_ = std::make_unique<query::ProductScan>(std::move(lhsScan_),
                                                   std::move(rhsScan_));
  nextBlkNum_ = end + 1;
  return true;
}

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql