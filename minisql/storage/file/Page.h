#pragma once

#include <common/ByteBuffer.h>
#include <memory>
#include <mutex>

namespace minisql {
namespace storage {
namespace file {

class Block;

class Page {
 public:
  static constexpr int32_t INT32_SIZE = sizeof(int32_t);
  static constexpr int32_t BLOCK_SIZE = 512;

 public:
  Page() = default;

  void read(const Block &blk);
  void write(const Block &blk);
  std::shared_ptr<Block> append(const std::string &fileName);

  int32_t getInt32(int32_t offset) const;
  std::string getString(int32_t offset) const;

  void setInt32(int32_t offset, int32_t val);
  void setString(int32_t offset, const std::string &s);

  static inline int32_t STR_SIZE(int32_t n) { return INT32_SIZE + n; }

 private:
  mutable std::mutex mutex_;
  common::ByteBuffer buf_{BLOCK_SIZE};
};

}  // namespace file
}  // namespace storage
}  // namespace minisql