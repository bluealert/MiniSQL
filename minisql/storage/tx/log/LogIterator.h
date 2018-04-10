#pragma once

#include <common/Iterator.h>
#include <storage/file/Page.h>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace tx {
namespace log {

class BasicLogRecord;

class LogIterator : public common::Iterator<BasicLogRecord> {
 public:
  explicit LogIterator(const std::shared_ptr<file::Block> &blk);

  bool hasNext() const override;
  std::unique_ptr<BasicLogRecord> next() override;

 private:
  void moveToNextBlock();

 private:
  file::Page pg_;
  int32_t currRecPos_;
  std::shared_ptr<file::Block> blk_;
};

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql