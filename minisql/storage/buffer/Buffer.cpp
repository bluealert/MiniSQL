#include <storage/buffer/Buffer.h>
#include <storage/buffer/PageFormatter.h>
#include <storage/tx/log/LogMgr.h>

namespace minisql {
namespace storage {
namespace buffer {

Buffer::Buffer() : pins_(0), modifiedBy_(-1), logSeqNum_(-1), blk_(nullptr) {}

int32_t Buffer::getInt32(int32_t offset) const {
  return page_.getInt32(offset);
}

void Buffer::setInt32(int32_t offset, int32_t val, int32_t txNum, int32_t lsn) {
  modifiedBy_ = txNum;
  if (lsn >= 0) {
    logSeqNum_ = lsn;
  }
  page_.setInt32(offset, val);
}

std::string Buffer::getString(int32_t offset) const {
  return page_.getString(offset);
}

void Buffer::setString(int32_t offset, const std::string &val, int32_t txNum,
                       int32_t lsn) {
  modifiedBy_ = txNum;
  if (lsn >= 0) {
    logSeqNum_ = lsn;
  }
  page_.setString(offset, val);
}

std::shared_ptr<file::Block> Buffer::block() const { return blk_; }

void Buffer::flush() {
  if (modifiedBy_ >= 0) {
    tx::log::LogMgr::get().flush(logSeqNum_);
    page_.write(*blk_);
    modifiedBy_ = -1;
  }
}

void Buffer::pin() { pins_++; }

void Buffer::unpin() { pins_--; }

bool Buffer::isPinned() const { return pins_ > 0; }

bool Buffer::isModifiedBy(int32_t txNum) const { return txNum == modifiedBy_; }

void Buffer::assignToBlock(std::shared_ptr<file::Block> blk) {
  flush();
  page_.read(*blk);
  blk_ = std::move(blk);
  pins_ = 0;
}

void Buffer::assignToNew(const std::string &fileName, PageFormatter &fmtr) {
  flush();
  fmtr.format(page_);
  blk_ = page_.append(fileName);
  pins_ = 0;
}

}  // namespace buffer
}  // namespace storage
}  // namespace minisql