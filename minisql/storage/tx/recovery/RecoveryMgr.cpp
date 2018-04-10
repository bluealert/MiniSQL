#include <common/Funcs.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/Block.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/CheckpointRecord.h>
#include <storage/tx/recovery/CommitRecord.h>
#include <storage/tx/recovery/LogRecordIterator.h>
#include <storage/tx/recovery/RecoveryMgr.h>
#include <storage/tx/recovery/RollbackRecord.h>
#include <storage/tx/recovery/SetInt32Record.h>
#include <storage/tx/recovery/SetStringRecord.h>
#include <storage/tx/recovery/StartRecord.h>
#include <unordered_set>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

bool isTempBlock(const file::Block &blk) {
  return common::startsWith(blk.fileName(), "temp");
}

void RecoveryMgr::start(int32_t txNum) {
  txNum_ = txNum;
  StartRecord rec(txNum);
  rec.writeToLog();
}

void RecoveryMgr::commit() {
  buffer::BufferMgr::get().flushAll(txNum_);
  CommitRecord rec(txNum_);
  auto lsn = rec.writeToLog();
  log::LogMgr::get().flush(lsn);
}

void RecoveryMgr::rollback() {
  doRollback();
  buffer::BufferMgr::get().flushAll(txNum_);
  RollbackRecord rec(txNum_);
  auto lsn = rec.writeToLog();
  log::LogMgr::get().flush(lsn);
}

void RecoveryMgr::recover() {
  doRecover();
  buffer::BufferMgr::get().flushAll(txNum_);
  CheckpointRecord rec;
  auto lsn = rec.writeToLog();
  log::LogMgr::get().flush(lsn);
}

int32_t RecoveryMgr::setInt32(const buffer::Buffer &buff, int32_t offset,
                              int32_t newVal) {
  auto oldVal = buff.getInt32(offset);
  auto blk = buff.block();
  if (isTempBlock(*blk)) {
    return -1;
  }
  SetInt32Record rec(txNum_, blk, offset, oldVal);
  return rec.writeToLog();
}

int32_t RecoveryMgr::setString(const buffer::Buffer &buff, int32_t offset,
                               const std::string &newVal) {
  auto oldVal = buff.getString(offset);
  auto blk = buff.block();
  if (isTempBlock(*blk)) {
    return -1;
  }
  SetStringRecord rec(txNum_, blk, offset, oldVal);
  return rec.writeToLog();
}

void RecoveryMgr::doRollback() {
  LogRecordIterator iter;
  while (iter.hasNext()) {
    auto rec = iter.next();
    if (rec->txNumber() == txNum_) {
      if (rec->op() == LogRecord::Type::CHECKPOINT) {
        return;
      }
      rec->undo(txNum_);
    }
  }
}

void RecoveryMgr::doRecover() {
  std::unordered_set<int32_t> finishedTxs;
  LogRecordIterator iter;
  while (iter.hasNext()) {
    auto rec = iter.next();
    if (rec->op() == LogRecord::Type::CHECKPOINT) {
      return;
    }
    if (rec->op() == LogRecord::Type::COMMIT ||
        rec->op() == LogRecord::Type::ROLLBACK) {
      finishedTxs.insert(rec->txNumber());
    } else if (finishedTxs.find(rec->txNumber()) == finishedTxs.end()) {
      rec->undo(txNum_);
    }
  }
}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql