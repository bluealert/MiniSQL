#pragma once

#include <memory>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {
namespace index {
namespace btree {

class DirEntry {
 public:
  DirEntry(const std::shared_ptr<common::Constant>& dataVal, int32_t blkNum);

  std::shared_ptr<common::Constant> dataVal();
  int32_t blockNumber();

 private:
  int32_t blkNum_;
  std::shared_ptr<common::Constant> dataVal_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql