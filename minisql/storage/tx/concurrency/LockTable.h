#pragma once

#include <condition_variable>
#include <unordered_map>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace tx {
namespace concurrency {

static const int32_t MAX_TIME{10000};

class LockTable {
 public:
  LockTable(const LockTable &) = delete;
  LockTable &operator=(const LockTable &) = delete;
  LockTable(LockTable &&) = delete;
  LockTable &operator=(LockTable &&) = delete;

  static LockTable &get();
  void init(int32_t maxWaitTime = MAX_TIME);

  void sLock(const std::shared_ptr<file::Block> &blk);
  void xLock(const std::shared_ptr<file::Block> &blk);
  void unlock(const std::shared_ptr<file::Block> &blk);

 private:
  LockTable();

  bool hasXLock(const std::shared_ptr<file::Block> &blk);
  bool hasOtherSLocks(const std::shared_ptr<file::Block> &blk);
  int32_t getLockVal(const std::shared_ptr<file::Block> &blk);

 private:
  std::mutex mutex_;
  int32_t maxWaitTime_;
  std::condition_variable cond_;
  std::unordered_map<std::shared_ptr<file::Block>, int32_t> locks_;
};

}  // namespace concurrency
}  // namespace tx
}  // namespace storage
}  // namespace minisql