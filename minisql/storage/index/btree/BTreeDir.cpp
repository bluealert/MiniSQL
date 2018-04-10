#include <common/Constant.h>
#include <storage/file/Block.h>
#include <storage/index/btree/BTreeDir.h>
#include <storage/index/btree/BTreePage.h>
#include <storage/index/btree/DirEntry.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

BTreeDir::BTreeDir(const std::shared_ptr<file::Block>& blk,
                   const std::shared_ptr<record::TableInfo>& ti,
                   tx::Transaction& tx)
    : ti_(ti), tx_(tx) {
  fileName_ = blk->fileName();
  contents_ = std::make_shared<BTreePage>(blk, ti_, tx_);
}

void BTreeDir::close() { contents_->close(); }

int32_t BTreeDir::search(const std::shared_ptr<common::Constant>& searchKey) {
  auto childBlk = findChildBlock(searchKey);
  while (contents_->getFlag() > 0) {
    contents_->close();
    contents_ = std::make_shared<BTreePage>(childBlk, ti_, tx_);
    childBlk = findChildBlock(searchKey);
  }
  return childBlk->number();
}

void BTreeDir::makeNewRoot(const std::shared_ptr<DirEntry>& e) {
  auto firstVal = contents_->getDataVal(0);
  auto level = contents_->getFlag();
  auto newBlk = contents_->split(0, level);
  auto oldRoot = std::make_shared<DirEntry>(firstVal, newBlk->number());
  insertEntry(oldRoot);
  insertEntry(e);
  contents_->setFlag(level + 1);
}

std::shared_ptr<DirEntry> BTreeDir::insert(const std::shared_ptr<DirEntry>& e) {
  if (contents_->getFlag() == 0) {
    return insertEntry(e);
  }
  auto childBlk = findChildBlock(e->dataVal());
  auto child = std::make_shared<BTreeDir>(childBlk, ti_, tx_);
  auto myEntry = child->insert(e);
  child->close();
  return myEntry != nullptr ? insertEntry(myEntry) : nullptr;
}

std::shared_ptr<DirEntry> BTreeDir::insertEntry(
    const std::shared_ptr<DirEntry>& e) {
  auto newSlot = 1 + contents_->findSlotBefore(e->dataVal());
  contents_->insertDir(newSlot, e->dataVal(), e->blockNumber());
  if (!contents_->isFull()) {
    return nullptr;
  }
  auto level = contents_->getFlag();
  auto splitPos = contents_->getNumRecs() / 2;
  auto splitVal = contents_->getDataVal(splitPos);
  auto newBlk = contents_->split(splitPos, level);
  return std::make_shared<DirEntry>(splitVal, newBlk->number());
}

std::shared_ptr<file::Block> BTreeDir::findChildBlock(
    const std::shared_ptr<common::Constant>& searchKey) {
  auto slot = contents_->findSlotBefore(searchKey);
  if (contents_->getDataVal(slot + 1)->equals(*searchKey)) {
    slot++;
  }
  auto blkNum = contents_->getChildNum(slot);
  return std::make_shared<file::Block>(fileName_, blkNum);
}

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql