#include <storage/tx/concurrency/LockAbortException.h>
#include <storage/tx/concurrency/LockTable.h>

namespace minisql {
namespace storage {
namespace tx {
namespace concurrency {

using namespace std::chrono;

LockTable::LockTable() { maxWaitTime_ = MAX_TIME; }

LockTable& LockTable::get() {
  static LockTable inst;
  return inst;
}

void LockTable::init(int32_t maxWaitTime) {
  maxWaitTime_ = maxWaitTime;
  locks_.clear();
}

void LockTable::sLock(const std::shared_ptr<file::Block>& blk) {
  std::unique_lock<std::mutex> lk(mutex_);
  const auto timeout = steady_clock::now() + milliseconds(maxWaitTime_);
  cond_.wait_until(lk, timeout, [&blk, this]() { return !hasXLock(blk); });
  if (hasXLock(blk)) {
    throw LockAbortException();
  }
  locks_[blk] = getLockVal(blk) + 1;
}

void LockTable::xLock(const std::shared_ptr<file::Block>& blk) {
  std::unique_lock<std::mutex> lk(mutex_);
  const auto timeout = steady_clock::now() + milliseconds(maxWaitTime_);
  cond_.wait_until(lk, timeout,
                   [&blk, this]() { return !hasOtherSLocks(blk); });
  if (hasOtherSLocks(blk)) {
    throw LockAbortException();
  }
  locks_[blk] = -1;
}

void LockTable::unlock(const std::shared_ptr<file::Block>& blk) {
  std::lock_guard<std::mutex> lk(mutex_);
  int32_t val = getLockVal(blk);
  if (val > 1) {
    locks_[blk] = val - 1;
  } else {
    locks_.erase(blk);
    cond_.notify_all();
  }
}

bool LockTable::hasXLock(const std::shared_ptr<file::Block>& blk) {
  return getLockVal(blk) < 0;
}

bool LockTable::hasOtherSLocks(const std::shared_ptr<file::Block>& blk) {
  return getLockVal(blk) > 1;
}

int32_t LockTable::getLockVal(const std::shared_ptr<file::Block>& blk) {
  auto it = locks_.find(blk);
  return it == locks_.end() ? 0 : it->second;
}

}  // namespace concurrency
}  // namespace tx
}  // namespace storage
}  // namespace minisql