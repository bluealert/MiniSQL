#include <common/Constant.h>
#include <common/Int32Constant.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/RollbackRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

using common::Constant;
using common::Int32Constant;

RollbackRecord::RollbackRecord(int32_t txNum) : txNum_(txNum) {}

RollbackRecord::RollbackRecord(std::unique_ptr<log::BasicLogRecord> rec) {
  txNum_ = rec->nextInt32();
}

int32_t RollbackRecord::writeToLog() {
  std::vector<std::unique_ptr<Constant>> rec;
  rec.emplace_back(std::make_unique<Int32Constant>((int32_t)Type::START));
  rec.emplace_back(std::make_unique<Int32Constant>(txNum_));
  return log::LogMgr::get().append(rec);
}

LogRecord::Type RollbackRecord::op() const { return LogRecord::Type::ROLLBACK; }

int32_t RollbackRecord::txNumber() const { return txNum_; }

void RollbackRecord::undo(int32_t txNum) {}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql