#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace tx {
namespace concurrency {

class ConcurrencyMgr {
 public:
  ConcurrencyMgr() = default;

  void sLock(const std::shared_ptr<file::Block>& blk);
  void xLock(const std::shared_ptr<file::Block>& blk);
  void release();

 private:
  bool hasXLock(const std::shared_ptr<file::Block>& blk);

 private:
  std::unordered_map<std::shared_ptr<file::Block>, char> locks_;
};

}  // namespace concurrency
}  // namespace tx
}  // namespace storage
}  // namespace minisql