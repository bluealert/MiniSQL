#pragma once

#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/recovery/LogRecord.h>
#include <memory>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

class StartRecord : public LogRecord {
 public:
  explicit StartRecord(int32_t txNum);
  explicit StartRecord(std::unique_ptr<log::BasicLogRecord> rec);

  int32_t writeToLog() override;
  LogRecord::Type op() const override;
  int32_t txNumber() const override;
  void undo(int32_t txNum) override;

 private:
  int32_t txNum_;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql