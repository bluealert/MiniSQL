#pragma once

#include <storage/tx/BufferList.h>
#include <storage/tx/concurrency/ConcurrencyMgr.h>
#include <storage/tx/recovery/RecoveryMgr.h>

namespace minisql {
namespace storage {

namespace buffer {
class PageFormatter;
}

namespace tx {

class Transaction {
 public:
  Transaction();

  void commit();
  void rollback();
  void recover();

  void pin(const std::shared_ptr<file::Block>& blk);
  void unpin(const std::shared_ptr<file::Block>& blk);
  std::shared_ptr<file::Block> pinNew(const std::string& fileName,
                                      buffer::PageFormatter& fmtr);

  int32_t getInt32(const std::shared_ptr<file::Block>& blk, int32_t offset);
  std::string getString(const std::shared_ptr<file::Block>& blk,
                        int32_t offset);

  void setInt32(const std::shared_ptr<file::Block>& blk, int32_t offset,
                int32_t val);
  void setString(const std::shared_ptr<file::Block>& blk, int32_t offset,
                 const std::string& val);

  int32_t blockCount(const std::string& fileName);

 private:
  int32_t txNum_;
  BufferList myBuffers_;
  recovery::RecoveryMgr recoveryMgr_;
  concurrency::ConcurrencyMgr concurMgr_;
};

}  // namespace tx
}  // namespace storage
}  // namespace minisql