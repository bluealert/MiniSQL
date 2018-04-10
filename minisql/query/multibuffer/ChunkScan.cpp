#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <query/multibuffer/ChunkScan.h>
#include <storage/file/Block.h>
#include <storage/record/RecordPage.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace query {
namespace multibuffer {

using namespace storage;

ChunkScan::ChunkScan(const std::shared_ptr<record::TableInfo> &ti,
                     int32_t startBlkNum, int32_t endBlkNum,
                     tx::Transaction &tx)
    : startBlkNum_(startBlkNum), endBlkNum_(endBlkNum), sch_(ti->schema()) {
  auto fileName = ti->fileName();
  for (int32_t i = startBlkNum; i <= endBlkNum; i++) {
    auto blk = std::make_shared<file::Block>(fileName, i);
    pages_.push_back(std::make_shared<record::RecordPage>(blk, ti, tx));
  }
  beforeFirst();
}

void ChunkScan::beforeFirst() { moveToBlock(startBlkNum_); }

bool ChunkScan::next() {
  while (true) {
    if (rp_->next()) {
      return true;
    }
    if (curr_ == endBlkNum_) {
      return false;
    }
    moveToBlock(curr_ + 1);
  }
}

void ChunkScan::close() {
  for (auto &r : pages_) {
    r->close();
  }
}

std::shared_ptr<common::Constant> ChunkScan::getVal(
    const std::string &fldName) const {
  if (sch_->type(fldName) == record::Schema::Type::INT32) {
    return std::make_shared<common::Int32Constant>(rp_->getInt32(fldName));
  } else {
    return std::make_shared<common::StringConstant>(rp_->getString(fldName));
  }
}

int32_t ChunkScan::getInt32(const std::string &fldName) const {
  return rp_->getInt32(fldName);
}

std::string ChunkScan::getString(const std::string &fldName) const {
  return rp_->getString(fldName);
}

bool ChunkScan::hasField(const std::string &fldName) const {
  return sch_->hasField(fldName);
}

void ChunkScan::moveToBlock(int32_t blkNum) {
  curr_ = blkNum;
  rp_ = pages_[curr_ - startBlkNum_];
  rp_->moveToSlot(-1);
}

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql