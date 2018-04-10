#pragma once

#include <storage/tx/recovery/LogRecord.h>
#include <memory>

namespace minisql {
namespace storage {

namespace file {
class Block;
}

namespace log {
class BasicLogRecord;
}

namespace tx {
namespace recovery {

class SetInt32Record : public LogRecord {
 public:
  SetInt32Record(int32_t txNum, const std::shared_ptr<file::Block> &blk,
                 int32_t offset, int32_t val);
  explicit SetInt32Record(std::unique_ptr<log::BasicLogRecord> rec);

  int32_t writeToLog() override;
  LogRecord::Type op() const override;
  int32_t txNumber() const override;
  void undo(int32_t txNum) override;

 private:
  int32_t txNum_, offset_, val_;
  std::shared_ptr<file::Block> blk_;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql