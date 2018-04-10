#pragma once

#include <storage/file/Page.h>

namespace minisql {
namespace storage {
namespace buffer {

class PageFormatter;

class Buffer {
 public:
  explicit Buffer();

  void pin();
  void unpin();
  bool isPinned() const;

  int32_t getInt32(int32_t offset) const;
  std::string getString(int32_t offset) const;

  void setInt32(int32_t offset, int32_t val, int32_t txNum, int32_t lsn);
  void setString(int32_t offset, const std::string &val, int32_t txNum,
                 int32_t lsn);

  std::shared_ptr<file::Block> block() const;

  void flush();
  bool isModifiedBy(int32_t txNum) const;

  void assignToBlock(std::shared_ptr<file::Block> blk);
  void assignToNew(const std::string &fileName, PageFormatter &fmtr);

 private:
  int32_t pins_;
  int32_t modifiedBy_;
  int32_t logSeqNum_;
  file::Page page_;
  std::shared_ptr<file::Block> blk_;
};

}  // namespace buffer
}  // namespace storage
}  // namespace minisql