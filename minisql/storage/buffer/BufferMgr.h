#pragma once

#include <storage/buffer/BasicBufferMgr.h>
#include <condition_variable>
#include <mutex>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace buffer {

class Buffer;
class PageFormatter;

static constexpr int32_t MAX_TIME{10000};

class BufferMgr {
 public:
  BufferMgr(const BufferMgr &) = delete;
  BufferMgr &operator=(const BufferMgr &) = delete;
  BufferMgr(BufferMgr &&) = delete;
  BufferMgr &operator=(BufferMgr &&) = delete;

  static BufferMgr &get();

  void init(int32_t num, int32_t maxWaitTime = MAX_TIME);

  std::shared_ptr<Buffer> pin(const std::shared_ptr<file::Block> &blk);
  std::shared_ptr<Buffer> pinNew(const std::string &fileName,
                                 PageFormatter &fmtr);
  void unpin(const std::shared_ptr<Buffer> &buff);

  void flushAll(int32_t txNum);
  int32_t available() const;

 private:
  BufferMgr() = default;

  int32_t maxWaitTime_;
  std::mutex mutex_;
  std::condition_variable cond_;
  BasicBufferMgr buffMgr_;
};

}  // namespace buffer
}  // namespace storage
}  // namespace minisql