#include <common/Constant.h>
#include <storage/file/Block.h>
#include <storage/index/btree/BTreeLeaf.h>
#include <storage/index/btree/BTreePage.h>
#include <storage/index/btree/DirEntry.h>
#include <storage/record/RID.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

BTreeLeaf::BTreeLeaf(const std::shared_ptr<file::Block>& blk,
                     const std::shared_ptr<record::TableInfo>& ti,
                     const std::shared_ptr<common::Constant>& searchKey,
                     tx::Transaction& tx)
    : tx_(tx), ti_(ti), searchKey_(searchKey) {
  contents_ = std::make_shared<BTreePage>(blk, ti_, tx_);
  currSlot_ = contents_->findSlotBefore(searchKey);
}

void BTreeLeaf::close() { contents_->close(); }

bool BTreeLeaf::next() {
  currSlot_++;
  if (currSlot_ >= contents_->getNumRecs()) {
    return tryOverflow();
  }
  if (contents_->getDataVal(currSlot_)->equals(*searchKey_)) {
    return true;
  }
  return tryOverflow();
}

std::unique_ptr<record::RID> BTreeLeaf::getDataRid() {
  return contents_->getDataRid(currSlot_);
}

void BTreeLeaf::del(const std::shared_ptr<record::RID>& dataRid) {
  while (next()) {
    if (getDataRid()->equals(*dataRid)) {
      contents_->del(currSlot_);
      return;
    }
  }
}

std::shared_ptr<DirEntry> BTreeLeaf::insert(
    const std::shared_ptr<record::RID>& dataRid) {
  if (contents_->getFlag() >= 0 &&
      contents_->getDataVal(0)->compareTo(*searchKey_) > 0) {
    auto firstVal = contents_->getDataVal(0);
    auto newBlk = contents_->split(0, contents_->getFlag());
    currSlot_ = 0;
    contents_->setFlag(-1);
    contents_->insertLeaf(currSlot_, searchKey_, dataRid);
    return std::make_shared<DirEntry>(firstVal, newBlk->number());
  }

  currSlot_++;
  contents_->insertLeaf(currSlot_, searchKey_, dataRid);
  if (!contents_->isFull()) {
    return nullptr;
  }

  auto firstKey = contents_->getDataVal(0);
  auto lastKey = contents_->getDataVal(contents_->getNumRecs() - 1);
  if (lastKey->equals(*firstKey)) {
    auto newBlk = contents_->split(1, contents_->getFlag());
    contents_->setFlag(newBlk->number());
    return nullptr;
  } else {
    auto splitPos = contents_->getNumRecs() / 2;
    auto splitKey = contents_->getDataVal(splitPos);
    if (splitKey->equals(*firstKey)) {
      while (contents_->getDataVal(splitPos)->equals(*splitKey)) {
        splitPos++;
      }
      splitKey = contents_->getDataVal(splitPos);
    } else {
      while (contents_->getDataVal(splitPos - 1)->equals(*splitKey)) {
        splitPos--;
      }
    }
    auto newBlk = contents_->split(splitPos, -1);
    return std::make_shared<DirEntry>(splitKey, newBlk->number());
  }
}

bool BTreeLeaf::tryOverflow() {
  auto firstKey = contents_->getDataVal(0);
  auto flag = contents_->getFlag();
  if (!searchKey_->equals(*firstKey) || flag < 0) {
    return false;
  }
  contents_->close();
  auto nextBlk = std::make_shared<file::Block>(ti_->fileName(), flag);
  contents_ = std::make_shared<BTreePage>(nextBlk, ti_, tx_);
  currSlot_ = 0;
  return true;
}

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql