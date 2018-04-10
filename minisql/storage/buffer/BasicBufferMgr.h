#pragma once

#include <memory>
#include <mutex>
#include <vector>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace buffer {

class Buffer;
class PageFormatter;

class BasicBufferMgr {
 public:
  BasicBufferMgr() = default;

  void init(int32_t num);

  BasicBufferMgr(const BasicBufferMgr &) = delete;
  BasicBufferMgr &operator=(const BasicBufferMgr &) = delete;
  BasicBufferMgr(BasicBufferMgr &&) = delete;
  BasicBufferMgr &operator=(BasicBufferMgr &&) = delete;

  std::shared_ptr<Buffer> pin(const std::shared_ptr<file::Block> &blk);
  std::shared_ptr<Buffer> pinNew(const std::string &fileName,
                                 PageFormatter &fmtr);
  void unpin(const std::shared_ptr<Buffer> &buff);

  void flushAll(int txNum);
  int32_t available() const;

 private:
  std::shared_ptr<Buffer> findExistingBuffer(
      const std::shared_ptr<file::Block> &blk) const;
  std::shared_ptr<Buffer> chooseUnpinnedBuffer() const;

 private:
  std::mutex mutex_;
  int32_t numAvailable_;
  std::vector<std::shared_ptr<Buffer>> bufferPool_;
};

}  // namespace buffer
}  // namespace storage
}  // namespace minisql