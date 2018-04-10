#pragma once

#include <deque>
#include <memory>
#include <unordered_map>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace buffer {
class Buffer;
class PageFormatter;
}  // namespace buffer

namespace tx {

class BufferList {
 public:
  std::shared_ptr<buffer::Buffer> getBuffer(
      const std::shared_ptr<file::Block>& blk);
  void pin(const std::shared_ptr<file::Block>& blk);
  std::shared_ptr<file::Block> pinNew(const std::string& fileName,
                                      buffer::PageFormatter& fmtr);
  void unpin(const std::shared_ptr<file::Block>& blk);
  void unpinAll();

 private:
  void eraseFromPins(const std::shared_ptr<file::Block>& blk);

 private:
  std::deque<std::shared_ptr<file::Block>> pins_;
  std::unordered_map<std::shared_ptr<file::Block>,
                     std::shared_ptr<buffer::Buffer>>
      buffers_;
};

}  // namespace tx
}  // namespace storage
}  // namespace minisql