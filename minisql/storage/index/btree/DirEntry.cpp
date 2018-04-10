#include <storage/index/btree/DirEntry.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

DirEntry::DirEntry(const std::shared_ptr<common::Constant>& dataVal,
                   int32_t blkNum)
    : blkNum_(blkNum), dataVal_(dataVal) {}

std::shared_ptr<common::Constant> DirEntry::dataVal() { return dataVal_; }

int32_t DirEntry::blockNumber() { return blkNum_; }

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql