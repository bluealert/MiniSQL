#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <storage/file/Block.h>
#include <storage/file/FileMgr.h>
#include <storage/file/Page.h>
#include <storage/tx/log/LogIterator.h>
#include <storage/tx/log/LogMgr.h>

namespace minisql {
namespace storage {
namespace tx {
namespace log {

LogMgr &LogMgr::get() {
  static LogMgr inst;
  return inst;
}

void LogMgr::init(std::string logFile) {
  logFile_ = std::move(logFile);
  currBlock_ = nullptr;
  auto blockCount = file::FileMgr::get().blockCount(logFile_);
  if (blockCount == 0) {
    appendNewBlock();
  } else {
    readLastBlock(blockCount);
  }
}

void LogMgr::flush(int32_t lsn) {
  if (lsn >= currentLSN()) {
    flush();
  }
}

void LogMgr::readLastBlock(int32_t blkCount) {
  currBlock_ = std::make_shared<file::Block>(logFile_, blkCount - 1);
  myPage_.read(*currBlock_);
  currPos_ = getLastRecordPosition() + file::Page::INT32_SIZE;
}

void LogMgr::appendNewBlock() {
  setLastRecordPosition(0);
  currPos_ = file::Page::INT32_SIZE;
  currBlock_ = myPage_.append(logFile_);
}

void LogMgr::finalizeRecord() {
  myPage_.setInt32(currPos_, getLastRecordPosition());
  setLastRecordPosition(currPos_);
  currPos_ += file::Page::INT32_SIZE;
}

int32_t LogMgr::getLastRecordPosition() const {
  return myPage_.getInt32(LAST_POS);
}

void LogMgr::setLastRecordPosition(int32_t pos) {
  myPage_.setInt32(LAST_POS, pos);
}

int32_t LogMgr::currentLSN() const { return currBlock_->number(); }

void LogMgr::flush() { myPage_.write(*currBlock_); }

std::unique_ptr<LogIterator> LogMgr::iterator() {
  std::lock_guard<std::mutex> lk(mutex_);
  flush();
  return std::make_unique<LogIterator>(currBlock_);
}

int32_t LogMgr::append(
    const std::vector<std::unique_ptr<common::Constant>> &rec) {
  std::lock_guard<std::mutex> lk(mutex_);
  auto recSize = file::Page::INT32_SIZE;
  for (auto &obj : rec) {
    recSize += obj->size();
  }
  if (currPos_ + recSize >= file::Page::BLOCK_SIZE) {
    flush();
    appendNewBlock();
  }
  for (auto &obj : rec) {
    appendVal(*obj);
  }
  finalizeRecord();
  return currentLSN();
}

void LogMgr::appendVal(common::Constant &val) {
  if (val.type() == common::Constant::Type::INTEGER) {
    auto ic = dynamic_cast<common::Int32Constant *>(&val);
    myPage_.setInt32(currPos_, ic->val());
  } else {
    auto sc = dynamic_cast<common::StringConstant *>(&val);
    myPage_.setString(currPos_, sc->val());
  }
  currPos_ += val.size();
}

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql