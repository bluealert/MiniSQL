#pragma once

#include <common/Iterator.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <memory>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

class LogRecord;

class LogRecordIterator : public common::Iterator<LogRecord> {
 public:
  LogRecordIterator();

  bool hasNext() const override;
  std::unique_ptr<LogRecord> next() override;

 private:
  std::unique_ptr<common::Iterator<log::BasicLogRecord>> iter_;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql