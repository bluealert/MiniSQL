#include <storage/tx/concurrency/ConcurrencyMgr.h>
#include <storage/tx/concurrency/LockTable.h>

namespace minisql {
namespace storage {
namespace tx {
namespace concurrency {

static constexpr char lockS = 'S', lockX = 'X';

void ConcurrencyMgr::sLock(const std::shared_ptr<file::Block>& blk) {
  if (locks_.find(blk) == locks_.end()) {
    LockTable::get().sLock(blk);
    locks_[blk] = lockS;
  }
}

void ConcurrencyMgr::xLock(const std::shared_ptr<file::Block>& blk) {
  if (!hasXLock(blk)) {
    sLock(blk);
    LockTable::get().xLock(blk);
    locks_[blk] = lockX;
  }
}

void ConcurrencyMgr::release() {
  for (auto& kv : locks_) {
    LockTable::get().unlock(kv.first);
  }
  locks_.clear();
}

bool ConcurrencyMgr::hasXLock(const std::shared_ptr<file::Block>& blk) {
  auto it = locks_.find(blk);
  return it != locks_.end() && it->second == lockX;
}

}  // namespace concurrency
}  // namespace tx
}  // namespace storage
}  // namespace minisql