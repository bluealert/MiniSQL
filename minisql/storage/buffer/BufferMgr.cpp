#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferAbortException.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/buffer/PageFormatter.h>

namespace minisql {
namespace storage {
namespace buffer {

using namespace std::chrono;

BufferMgr &BufferMgr::get() {
  static BufferMgr inst;
  return inst;
}

void BufferMgr::init(int32_t num, int32_t maxWaitTime) {
  buffMgr_.init(num);
  maxWaitTime_ = maxWaitTime;
}

std::shared_ptr<Buffer> BufferMgr::pin(
    const std::shared_ptr<file::Block> &blk) {
  std::unique_lock<std::mutex> lk(mutex_);
  const auto timeout = steady_clock::now() + milliseconds(maxWaitTime_);
  std::shared_ptr<Buffer> buff = nullptr;
  cond_.wait_until(lk, timeout, [&blk, &buff, this]() {
    buff = buffMgr_.pin(blk);
    return buff != nullptr;
  });
  if (buff == nullptr) {
    throw BufferAbortException();
  }
  return buff;
}

std::shared_ptr<Buffer> BufferMgr::pinNew(const std::string &fileName,
                                          PageFormatter &fmtr) {
  std::unique_lock<std::mutex> lk(mutex_);
  const auto timeout = steady_clock::now() + milliseconds(maxWaitTime_);
  std::shared_ptr<Buffer> buff = nullptr;
  cond_.wait_until(lk, timeout, [&fileName, &fmtr, &buff, this]() {
    buff = buffMgr_.pinNew(fileName, fmtr);
    return buff != nullptr;
  });
  if (buff == nullptr) {
    throw BufferAbortException();
  }
  return buff;
}

void BufferMgr::unpin(const std::shared_ptr<Buffer> &buff) {
  std::lock_guard<std::mutex> lk(mutex_);
  buffMgr_.unpin(buff);
  if (!buff->isPinned()) {
    cond_.notify_all();
  }
}

void BufferMgr::flushAll(int32_t txNum) { buffMgr_.flushAll(txNum); }

int32_t BufferMgr::available() const { return buffMgr_.available(); }

}  // namespace buffer
}  // namespace storage
}  // namespace minisql