#include <common/Int32Constant.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/StartRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

using common::Constant;
using common::Int32Constant;

StartRecord::StartRecord(int32_t txNum) : txNum_(txNum) {}

StartRecord::StartRecord(std::unique_ptr<log::BasicLogRecord> rec) {
  txNum_ = rec->nextInt32();
}

int32_t StartRecord::writeToLog() {
  std::vector<std::unique_ptr<Constant>> rec;
  rec.emplace_back(std::make_unique<Int32Constant>((int32_t)Type::START));
  rec.emplace_back(std::make_unique<Int32Constant>(txNum_));
  return log::LogMgr::get().append(rec);
}

LogRecord::Type StartRecord::op() const { return Type::START; }

int32_t StartRecord::txNumber() const { return txNum_; }

void StartRecord::undo(int32_t txNum) {}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql