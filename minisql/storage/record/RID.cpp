#include <storage/record/RID.h>

namespace minisql {
namespace storage {
namespace record {

RID::RID(int32_t blkNum, int32_t slotNum)
    : blkNum_(blkNum), slotNum_(slotNum) {}

int32_t RID::blockNumber() const { return blkNum_; }

int32_t RID::slotNumber() const { return slotNum_; }

bool RID::equals(const RID &rhs) const {
  return blkNum_ == rhs.blkNum_ && slotNum_ == rhs.slotNum_;
}

}  // namespace record
}  // namespace storage
}  // namespace minisql