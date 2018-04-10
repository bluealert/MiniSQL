#include <storage/file/Block.h>

namespace minisql {
namespace storage {
namespace file {

Block::Block(std::string fileName, int blkNum)
    : fileName_(std::move(fileName)), blkNum_(blkNum) {}

std::string Block::fileName() const { return fileName_; }

int Block::number() const { return blkNum_; }

bool Block::equals(const Block &rhs) const {
  return fileName_ == rhs.fileName_ && blkNum_ == rhs.blkNum_;
}

}  // namespace file
}  // namespace storage
}  // namespace minisql