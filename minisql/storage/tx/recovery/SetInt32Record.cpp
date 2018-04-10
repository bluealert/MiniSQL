#include <common/Constant.h>
#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/Block.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/SetInt32Record.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

using common::Constant;
using common::Int32Constant;
using common::StringConstant;

SetInt32Record::SetInt32Record(int32_t txNum,
                               const std::shared_ptr<file::Block> &blk,
                               int32_t offset, int32_t val)
    : txNum_(txNum), offset_(offset), val_(val), blk_(blk) {}

SetInt32Record::SetInt32Record(std::unique_ptr<log::BasicLogRecord> rec) {
  txNum_ = rec->nextInt32();
  auto fileName = rec->nextString();
  int32_t blkNum = rec->nextInt32();
  blk_ = std::make_shared<file::Block>(fileName, blkNum);
  offset_ = rec->nextInt32();
  val_ = rec->nextInt32();
}

int32_t SetInt32Record::writeToLog() {
  std::vector<std::unique_ptr<Constant>> rec;
  rec.emplace_back(std::make_unique<Int32Constant>((int32_t)Type::SETINT32));
  rec.emplace_back(std::make_unique<Int32Constant>(txNum_));
  rec.emplace_back(std::make_unique<StringConstant>(blk_->fileName()));
  rec.emplace_back(std::make_unique<Int32Constant>(blk_->number()));
  rec.emplace_back(std::make_unique<Int32Constant>(offset_));
  rec.emplace_back(std::make_unique<Int32Constant>(val_));
  return log::LogMgr::get().append(rec);
}

LogRecord::Type SetInt32Record::op() const { return Type::SETINT32; }

int32_t SetInt32Record::txNumber() const { return txNum_; }

void SetInt32Record::undo(int32_t txNum) {
  auto buff = buffer::BufferMgr::get().pin(blk_);
  buff->setInt32(offset_, val_, txNum, -1);
  buffer::BufferMgr::get().unpin(buff);
}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql