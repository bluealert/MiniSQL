#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/CheckpointRecord.h>
#include <storage/tx/recovery/CommitRecord.h>
#include <storage/tx/recovery/LogRecordIterator.h>
#include <storage/tx/recovery/RollbackRecord.h>
#include <storage/tx/recovery/SetInt32Record.h>
#include <storage/tx/recovery/SetStringRecord.h>
#include <storage/tx/recovery/StartRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

LogRecordIterator::LogRecordIterator() {
  iter_ = storage::tx::log::LogMgr::get().iterator();
}

bool LogRecordIterator::hasNext() const { return iter_->hasNext(); }

std::unique_ptr<LogRecord> LogRecordIterator::next() {
  auto rec = iter_->next();
  auto op = (LogRecord::Type)rec->nextInt32();
  switch (op) {
    case LogRecord::Type::START:
      return std::make_unique<StartRecord>(std::move(rec));
    case LogRecord::Type::SETINT32:
      return std::make_unique<SetInt32Record>(std::move(rec));
    case LogRecord::Type::SETSTRING:
      return std::make_unique<SetStringRecord>(std::move(rec));
    case LogRecord::Type::COMMIT:
      return std::make_unique<CommitRecord>(std::move(rec));
    case LogRecord::Type::ROLLBACK:
      return std::make_unique<RollbackRecord>(std::move(rec));
    case LogRecord::Type::CHECKPOINT:;
      return std::make_unique<CheckpointRecord>(std::move(rec));
  }
}

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql