#include <storage/file/Page.h>
#include <storage/record/RecordPage.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace storage {
namespace record {

RecordPage::RecordPage(const std::shared_ptr<file::Block> &blk,
                       const std::shared_ptr<TableInfo> &ti,
                       tx::Transaction &tx)
    : currSlot_(-1), tx_(tx), ti_(ti), blk_(blk) {
  slotSize_ = ti_->recordLength() + file::Page::INT32_SIZE;
  tx_.pin(blk);
}

void RecordPage::close() {
  if (blk_ != nullptr) {
    tx_.unpin(blk_);
    blk_ = nullptr;
  }
}

bool RecordPage::next() { return searchFor(INUSE); }

int32_t RecordPage::getInt32(const std::string &fldName) {
  return tx_.getInt32(blk_, fieldPos(fldName));
}

std::string RecordPage::getString(const std::string &fldName) {
  return tx_.getString(blk_, fieldPos(fldName));
}

void RecordPage::setInt32(const std::string &fldName, int32_t val) {
  tx_.setInt32(blk_, fieldPos(fldName), val);
}

void RecordPage::setString(const std::string &fldName, const std::string &val) {
  tx_.setString(blk_, fieldPos(fldName), val);
}

void RecordPage::del() { tx_.setInt32(blk_, currentPos(), EMPTY); }

bool RecordPage::insert() {
  currSlot_ = -1;
  bool found = searchFor(EMPTY);
  if (found) {
    int32_t pos = currentPos();
    tx_.setInt32(blk_, pos, INUSE);
  }
  return found;
}

void RecordPage::moveToSlot(int32_t slot) { currSlot_ = slot; }

int32_t RecordPage::currentSlot() const { return currSlot_; }

int32_t RecordPage::currentPos() const { return currSlot_ * slotSize_; }

int32_t RecordPage::fieldPos(const std::string &fldName) const {
  int32_t offset = file::Page::INT32_SIZE + ti_->offset(fldName);
  return currentPos() + offset;
}

bool RecordPage::isValidSlot() const {
  return currentPos() + slotSize_ <= file::Page::BLOCK_SIZE;
}

bool RecordPage::searchFor(int32_t flag) {
  currSlot_++;
  while (isValidSlot()) {
    int32_t pos = currentPos();
    if (tx_.getInt32(blk_, pos) == flag) {
      return true;
    }
    currSlot_++;
  }
  return false;
}

}  // namespace record
}  // namespace storage
}  // namespace minisql