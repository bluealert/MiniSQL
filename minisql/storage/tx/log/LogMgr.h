#pragma once

#include <common/Constant.h>
#include <storage/file/Page.h>
#include <storage/tx/log/LogIterator.h>
#include <vector>

namespace minisql {
namespace storage {
namespace tx {
namespace log {

class LogMgr {
 public:
  LogMgr(const LogMgr &) = delete;
  LogMgr &operator=(const LogMgr &) = delete;
  LogMgr(LogMgr &&) = delete;
  LogMgr &operator=(LogMgr &&) = delete;

  static const int LAST_POS = 0;

  static LogMgr &get();

  void init(std::string logFile);
  void flush(int32_t lsn);
  std::unique_ptr<LogIterator> iterator();
  int32_t append(const std::vector<std::unique_ptr<common::Constant>> &rec);

 private:
  LogMgr() = default;

  void appendNewBlock();
  void readLastBlock(int32_t blkCount);

  int32_t currentLSN() const;
  void flush();

  void appendVal(common::Constant &val);

  void finalizeRecord();
  int32_t getLastRecordPosition() const;
  void setLastRecordPosition(int32_t pos);

 private:
  int32_t currPos_{0};
  mutable std::mutex mutex_;
  std::string logFile_;
  file::Page myPage_;
  std::shared_ptr<file::Block> currBlock_;
};

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql