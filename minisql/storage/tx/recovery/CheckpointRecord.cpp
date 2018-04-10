#include <common/Constant.h>
#include <common/Int32Constant.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/CheckpointRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

using common::Constant;
using common::Int32Constant;

CheckpointRecord::CheckpointRecord(std::unique_ptr<log::BasicLogRecord> rec) {}

int32_t CheckpointRecord::writeToLog() {
  std::vector<std::unique_ptr<Constant>> rec;
  rec.emplace_back(std::make_unique<Int32Constant>((int32_t)Type::START));
  return log::LogMgr::get().append(rec);
}

LogRecord::Type CheckpointRecord::op() const { return Type::CHECKPOINT; }

int32_t CheckpointRecord::txNumber() const { return -1; }

void CheckpointRecord::undo(int32_t txNum) {}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql