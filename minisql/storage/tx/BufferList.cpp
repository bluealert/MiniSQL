#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/buffer/PageFormatter.h>
#include <storage/tx/BufferList.h>

namespace minisql {
namespace storage {
namespace tx {

std::shared_ptr<buffer::Buffer> BufferList::getBuffer(
    const std::shared_ptr<file::Block>& blk) {
  return buffers_[blk];
}

void BufferList::pin(const std::shared_ptr<file::Block>& blk) {
  auto buff = buffer::BufferMgr::get().pin(blk);
  buffers_[blk] = buff;
  pins_.push_back(blk);
}

std::shared_ptr<file::Block> BufferList::pinNew(const std::string& fileName,
                                                buffer::PageFormatter& fmtr) {
  auto buff = buffer::BufferMgr::get().pinNew(fileName, fmtr);
  auto blk = buff->block();
  buffers_.emplace(std::make_pair(blk, buff));
  return blk;
}

void BufferList::unpin(const std::shared_ptr<file::Block>& blk) {
  auto buff = buffers_[blk];
  buffer::BufferMgr::get().unpin(buff);
  eraseFromPins(blk);
  if (std::find(pins_.begin(), pins_.end(), blk) == pins_.end()) {
    buffers_.erase(blk);
  }
}

void BufferList::unpinAll() {
  for (const auto& blk : pins_) {
    auto buff = buffers_[blk];
    buffer::BufferMgr::get().unpin(buff);
  }
  buffers_.clear();
  pins_.clear();
}

void BufferList::eraseFromPins(const std::shared_ptr<file::Block>& blk) {
  auto it = std::find(pins_.begin(), pins_.end(), blk);
  if (it != pins_.end()) {
    pins_.erase(it);
  }
}

}  // namespace tx
}  // namespace storage
}  // namespace minisql