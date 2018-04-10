#include <common/Constant.h>
#include <common/Int32Constant.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/CommitRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

using common::Constant;
using common::Int32Constant;

CommitRecord::CommitRecord(int32_t txNum) : txNum_(txNum) {}

CommitRecord::CommitRecord(std::unique_ptr<log::BasicLogRecord> rec) {
  txNum_ = rec->nextInt32();
}

int32_t CommitRecord::writeToLog() {
  std::vector<std::unique_ptr<Constant>> rec;
  rec.emplace_back(std::make_unique<Int32Constant>((int32_t)Type::COMMIT));
  rec.emplace_back(std::make_unique<Int32Constant>(txNum_));
  return log::LogMgr::get().append(rec);
}

LogRecord::Type CommitRecord::op() const { return Type::COMMIT; }

int32_t CommitRecord::txNumber() const { return txNum_; }

void CommitRecord::undo(int32_t txNum) {}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql