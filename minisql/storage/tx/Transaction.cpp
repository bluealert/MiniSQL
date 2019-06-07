#include <atomic>
#include <glog/logging.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/buffer/PageFormatter.h>
#include <storage/file/Block.h>
#include <storage/file/FileMgr.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace storage {
namespace tx {

static const int32_t END_OF_FILE = -1;

static int32_t nextTxNumber() {
  static std::atomic<int32_t> nextTxNum{0};

  nextTxNum.store(nextTxNum + 1);
  int32_t txNum = nextTxNum.load();
  LOG(INFO) << "Transaction. new transaction: " << txNum;
  return txNum;
}

Transaction::Transaction() {
  txNum_ = nextTxNumber();
  recoveryMgr_.start(txNum_);
}

void Transaction::commit() {
  recoveryMgr_.commit();
  concurMgr_.release();
  myBuffers_.unpinAll();
  LOG(INFO) << "Transaction. " << txNum_ << " committed";
}

void Transaction::rollback() {
  recoveryMgr_.rollback();
  concurMgr_.release();
  myBuffers_.unpinAll();
  LOG(INFO) << "Transaction. " << txNum_ << " rolled back";
}

void Transaction::recover() {
  buffer::BufferMgr::get().flushAll(txNum_);
  recoveryMgr_.recover();
}

void Transaction::pin(const std::shared_ptr<file::Block>& blk) {
  myBuffers_.pin(blk);
}

void Transaction::unpin(const std::shared_ptr<file::Block>& blk) {
  myBuffers_.unpin(blk);
}

std::shared_ptr<file::Block> Transaction::pinNew(const std::string& fileName,
                                                 buffer::PageFormatter& fmtr) {
  auto dummyBlk = std::make_shared<file::Block>(fileName, END_OF_FILE);
  concurMgr_.xLock(dummyBlk);
  auto blk = myBuffers_.pinNew(fileName, fmtr);
  unpin(blk);
  return blk;
}

int32_t Transaction::getInt32(const std::shared_ptr<file::Block>& blk,
                              int32_t offset) {
  concurMgr_.sLock(blk);
  auto buff = myBuffers_.getBuffer(blk);
  return buff->getInt32(offset);
}

std::string Transaction::getString(const std::shared_ptr<file::Block>& blk,
                                   int32_t offset) {
  concurMgr_.sLock(blk);
  auto buff = myBuffers_.getBuffer(blk);
  return buff->getString(offset);
}

void Transaction::setInt32(const std::shared_ptr<file::Block>& blk,
                           int32_t offset, int32_t val) {
  concurMgr_.xLock(blk);
  auto buff = myBuffers_.getBuffer(blk);
  int32_t lsn = recoveryMgr_.setInt32(*buff, offset, val);
  buff->setInt32(offset, val, txNum_, lsn);
}

void Transaction::setString(const std::shared_ptr<file::Block>& blk,
                            int32_t offset, const std::string& val) {
  concurMgr_.xLock(blk);
  auto buff = myBuffers_.getBuffer(blk);
  int32_t lsn = recoveryMgr_.setString(*buff, offset, val);
  buff->setString(offset, val, txNum_, lsn);
}

int32_t Transaction::blockCount(const std::string& fileName) {
  auto dummyBlk = std::make_shared<file::Block>(fileName, END_OF_FILE);
  concurMgr_.sLock(dummyBlk);
  return file::FileMgr::get().blockCount(fileName);
}

}  // namespace tx
}  // namespace storage
}  // namespace minisql