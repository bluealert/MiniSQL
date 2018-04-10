#pragma once

#include <string>

namespace minisql {
namespace storage {
namespace file {

class Block {
 public:
  explicit Block(std::string fileName, int blkNum);

  std::string fileName() const;
  int number() const;
  bool equals(const Block &rhs) const;

 private:
  std::string fileName_;
  int blkNum_;
};

}  // namespace file
}  // namespace storage
}  // namespace minisql