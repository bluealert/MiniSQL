#include <storage/file/Block.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogIterator.h>
#include <storage/tx/log/LogMgr.h>

namespace minisql {
namespace storage {
namespace tx {
namespace log {

LogIterator::LogIterator(const std::shared_ptr<file::Block>& blk) : blk_(blk) {
  pg_.read(*blk);
  currRecPos_ = pg_.getInt32(LogMgr::LAST_POS);
}

bool LogIterator::hasNext() const {
  return currRecPos_ > 0 || blk_->number() > 0;
}

std::unique_ptr<BasicLogRecord> LogIterator::next() {
  if (currRecPos_ == 0) {
    moveToNextBlock();
  }
  currRecPos_ = pg_.getInt32(currRecPos_);
  return std::make_unique<BasicLogRecord>(pg_,
                                          currRecPos_ + file::Page::INT32_SIZE);
}

void LogIterator::moveToNextBlock() {
  blk_ = std::make_shared<file::Block>(blk_->fileName(), blk_->number() - 1);
  pg_.read(*blk_);
  currRecPos_ = pg_.getInt32(LogMgr::LAST_POS);
}

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql