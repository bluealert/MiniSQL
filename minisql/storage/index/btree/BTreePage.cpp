#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <storage/file/Page.h>
#include <storage/index/btree/BTPageFormatter.h>
#include <storage/index/btree/BTreePage.h>
#include <storage/record/RID.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

BTreePage::BTreePage(const std::shared_ptr<file::Block> &currBlk,
                     const std::shared_ptr<record::TableInfo> &ti,
                     tx::Transaction &tx)
    : currBlk_(currBlk), ti_(ti), tx_(tx) {
  slotSize_ = ti_->recordLength();
  tx_.pin(currBlk_);
}

int32_t BTreePage::findSlotBefore(
    const std::shared_ptr<common::Constant> &searchKey) {
  int32_t slot = 0;
  while (slot < getNumRecs() && getDataVal(slot)->compareTo(*searchKey) < 0) {
    slot++;
  }
  return slot - 1;
}

void BTreePage::close() {
  if (currBlk_ != nullptr) {
    tx_.unpin(currBlk_);
  }
  currBlk_ = nullptr;
}

bool BTreePage::isFull() {
  return slotPos(getNumRecs() + 1) >= file::Page::BLOCK_SIZE;
}

std::shared_ptr<file::Block> BTreePage::split(int32_t splitPos, int32_t flag) {
  auto newBlk = appendNew(flag);
  auto newPage = std::make_unique<BTreePage>(newBlk, ti_, tx_);
  transferRecs(splitPos, *newPage);
  newPage->setFlag(flag);
  newPage->close();
  return newBlk;
}

std::shared_ptr<common::Constant> BTreePage::getDataVal(int32_t slot) {
  return getVal(slot, "dataval");
}

int32_t BTreePage::getFlag() { return tx_.getInt32(currBlk_, 0); }

void BTreePage::setFlag(int32_t val) { tx_.setInt32(currBlk_, 0, val); }

int32_t BTreePage::getChildNum(int32_t slot) { return getInt32(slot, "block"); }

void BTreePage::insertDir(int32_t slot,
                          const std::shared_ptr<common::Constant> &val,
                          int32_t blkNum) {
  insert(slot);
  setVal(slot, "dataval", val);
  setInt32(slot, "block", blkNum);
}

std::unique_ptr<record::RID> BTreePage::getDataRid(int32_t slot) {
  return std::make_unique<record::RID>(getInt32(slot, "block"),
                                       getInt32(slot, "id"));
}

void BTreePage::insertLeaf(int32_t slot,
                           const std::shared_ptr<common::Constant> &val,
                           const std::shared_ptr<record::RID> &rid) {
  insert(slot);
  setVal(slot, "dataval", val);
  setInt32(slot, "block", rid->blockNumber());
  setInt32(slot, "id", rid->slotNumber());
}

void BTreePage::del(int32_t slot) {
  for (auto i = slot + 1; i < getNumRecs(); i++) {
    copyRecord(i, i - 1);
  }
  setNumRecs(getNumRecs() - 1);
}

int32_t BTreePage::getNumRecs() {
  return tx_.getInt32(currBlk_, file::Page::INT32_SIZE);
}

std::shared_ptr<file::Block> BTreePage::appendNew(int32_t flag) {
  auto fmtr = std::make_unique<BTPageFormatter>(*ti_, flag);
  return tx_.pinNew(ti_->fileName(), *fmtr);
}

int32_t BTreePage::getInt32(int32_t slot, const std::string &fldName) {
  auto pos = fldPos(slot, fldName);
  return tx_.getInt32(currBlk_, pos);
}

std::string BTreePage::getString(int32_t slot, const std::string &fldName) {
  auto pos = fldPos(slot, fldName);
  return tx_.getString(currBlk_, pos);
}

std::shared_ptr<common::Constant> BTreePage::getVal(
    int32_t slot, const std::string &fldName) {
  auto type = ti_->schema()->type(fldName);
  if (type == record::Schema::Type::INT32) {
    return std::make_shared<common::Int32Constant>(getInt32(slot, fldName));
  } else {
    return std::make_shared<common::StringConstant>(getString(slot, fldName));
  }
}

void BTreePage::setInt32(int32_t slot, const std::string &fldName,
                         int32_t val) {
  auto pos = fldPos(slot, fldName);
  tx_.setInt32(currBlk_, pos, val);
}

void BTreePage::setString(int32_t slot, const std::string &fldName,
                          const std::string &val) {
  auto pos = fldPos(slot, fldName);
  tx_.setString(currBlk_, pos, val);
}

void BTreePage::setVal(int32_t slot, const std::string &fldName,
                       const std::shared_ptr<common::Constant> &val) {
  auto type = ti_->schema()->type(fldName);
  if (type == record::Schema::Type::INT32) {
    setInt32(slot, fldName,
             std::dynamic_pointer_cast<common::Int32Constant>(val)->val());
  } else {
    setString(slot, fldName,
              std::dynamic_pointer_cast<common::StringConstant>(val)->val());
  }
}

void BTreePage::setNumRecs(int32_t n) {
  tx_.setInt32(currBlk_, file::Page::INT32_SIZE, n);
}

void BTreePage::insert(int32_t slot) {
  for (auto i = getNumRecs(); i > slot; i--) {
    copyRecord(i - 1, i);
  }
  setNumRecs(getNumRecs() + 1);
}

void BTreePage::copyRecord(int32_t from, int32_t to) {
  auto sch = ti_->schema();
  for (auto &fldName : sch->fields()) {
    setVal(to, fldName, getVal(from, fldName));
  }
}

void BTreePage::transferRecs(int32_t slot, BTreePage &dest) {
  int32_t destSlot = 0;
  while (slot < getNumRecs()) {
    dest.insert(destSlot);
    auto sch = ti_->schema();
    for (auto &fldName : sch->fields()) {
      dest.setVal(destSlot, fldName, getVal(slot, fldName));
    }
    del(slot);
    destSlot++;
  }
}

int32_t BTreePage::fldPos(int32_t slot, const std::string &fldName) {
  auto offset = ti_->offset(fldName);
  return slotPos(slot) + offset;
}

int32_t BTreePage::slotPos(int32_t slot) {
  return file::Page::INT32_SIZE * 2 + slot * slotSize_;
}

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql