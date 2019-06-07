#include <algorithm>
#include <storage/buffer/BasicBufferMgr.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/PageFormatter.h>
#include <storage/file/Block.h>

namespace minisql {
namespace storage {
namespace buffer {

void BasicBufferMgr::init(int32_t num) {
  numAvailable_ = num;
  bufferPool_.clear();
  bufferPool_.reserve((size_t)num);
  for (auto i = 0; i < num; i++) {
    bufferPool_.emplace_back(std::make_shared<Buffer>());
  }
}

void BasicBufferMgr::flushAll(int32_t txNum) {
  for (const auto &buff : bufferPool_) {
    if (buff->isModifiedBy(txNum)) {
      buff->flush();
    }
  }
}

std::shared_ptr<Buffer> BasicBufferMgr::pin(
    const std::shared_ptr<file::Block> &blk) {
  std::lock_guard<std::mutex> lk(mutex_);
  auto buff = findExistingBuffer(blk);
  if (buff == nullptr) {
    buff = chooseUnpinnedBuffer();
    if (buff == nullptr) {
      return nullptr;
    }
    buff->assignToBlock(blk);
  }
  if (!buff->isPinned()) {
    numAvailable_--;
  }
  buff->pin();
  return buff;
}

std::shared_ptr<Buffer> BasicBufferMgr::pinNew(const std::string &fileName,
                                               PageFormatter &fmtr) {
  std::lock_guard<std::mutex> lk(mutex_);
  auto buff = chooseUnpinnedBuffer();
  if (buff == nullptr) {
    return nullptr;
  }
  buff->assignToNew(fileName, fmtr);
  numAvailable_--;
  buff->pin();
  return buff;
}

void BasicBufferMgr::unpin(const std::shared_ptr<Buffer> &buff) {
  std::lock_guard<std::mutex> lk(mutex_);
  buff->unpin();
  if (!buff->isPinned()) {
    numAvailable_--;
  }
}

int32_t BasicBufferMgr::available() const { return numAvailable_; }

std::shared_ptr<Buffer> BasicBufferMgr::findExistingBuffer(
    const std::shared_ptr<file::Block> &blk) const {
  auto pred = [&blk](auto buff) {
    auto b = buff->block();
    return b != nullptr && blk->equals(*b);
  };
  auto it = std::find_if(bufferPool_.begin(), bufferPool_.end(), pred);
  return it != bufferPool_.end() ? *it : nullptr;
}

std::shared_ptr<Buffer> BasicBufferMgr::chooseUnpinnedBuffer() const {
  auto pred = [](auto buff) { return !buff->isPinned(); };
  auto it = std::find_if(bufferPool_.begin(), bufferPool_.end(), pred);
  return it != bufferPool_.end() ? *it : nullptr;
}

}  // namespace buffer
}  // namespace storage
}  // namespace minisql