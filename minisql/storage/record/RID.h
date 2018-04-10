#pragma once

#include <cstdint>

namespace minisql {
namespace storage {
namespace record {

class RID {
 public:
  RID(int32_t blkNum, int32_t slotNum);

  int32_t blockNumber() const;
  int32_t slotNumber() const;
  bool equals(const RID &rhs) const;

 private:
  int32_t blkNum_, slotNum_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql