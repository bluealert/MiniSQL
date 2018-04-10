#include <storage/file/Block.h>
#include <storage/record/RecordFile.h>
#include <storage/record/RecordPageFormatter.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace storage {
namespace record {

RecordFile::RecordFile(const std::shared_ptr<TableInfo> &ti,
                       tx::Transaction &tx)
    : ti_(ti), tx_(tx), fileName_(ti->fileName()), rp_(nullptr) {
  if (tx_.blockCount(fileName_) == 0) {
    appendBlock();
  }
  moveToBlock(0);
}

RecordFile::~RecordFile() {
  if (rp_ != nullptr) {
    rp_->close();
  }
}

void RecordFile::close() {
  if (rp_ != nullptr) {
    rp_->close();
  }
}

void RecordFile::beforeFirst() { moveToBlock(0); }

bool RecordFile::next() {
  while (true) {
    if (rp_->next()) {
      return true;
    }
    if (atLastBlock()) {
      return false;
    }
    moveToBlock(currBlkNum_ + 1);
  }
}

int32_t RecordFile::getInt32(const std::string &fldName) const {
  return rp_->getInt32(fldName);
}

std::string RecordFile::getString(const std::string &fldName) const {
  return rp_->getString(fldName);
}

void RecordFile::setInt32(const std::string &fldName, int32_t val) {
  rp_->setInt32(fldName, val);
}

void RecordFile::setString(const std::string &fldName, const std::string &val) {
  rp_->setString(fldName, val);
}

void RecordFile::del() { rp_->del(); }

void RecordFile::insert() {
  while (!rp_->insert()) {
    if (atLastBlock()) {
      appendBlock();
    }
    moveToBlock(currBlkNum_ + 1);
  }
}

void RecordFile::moveToRid(const RID &rid) {
  moveToBlock(rid.blockNumber());
  rp_->moveToSlot(rid.slotNumber());
}

std::unique_ptr<RID> RecordFile::currentRid() const {
  int32_t id = rp_->currentSlot();
  return std::make_unique<RID>(currBlkNum_, id);
}

void RecordFile::moveToBlock(int32_t b) {
  if (rp_ != nullptr) {
    rp_->close();
  }
  currBlkNum_ = b;
  auto blk = std::make_shared<file::Block>(fileName_, currBlkNum_);
  rp_ = std::make_unique<RecordPage>(blk, ti_, tx_);
}

bool RecordFile::atLastBlock() {
  auto blockCount = tx_.blockCount(fileName_);
  return currBlkNum_ == blockCount - 1;
}

void RecordFile::appendBlock() {
  RecordPageFormatter fmtr(ti_);
  tx_.pinNew(fileName_, fmtr);
}

}  // namespace record
}  // namespace storage
}  // namespace minisql