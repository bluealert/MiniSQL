#pragma once

#include <storage/tx/recovery/LogRecord.h>
#include <memory>
#include <string>

namespace minisql {
namespace storage {

namespace log {
class BasicLogRecord;
}

namespace file {
class Block;
}

namespace tx {
namespace recovery {

class SetStringRecord : public LogRecord {
 public:
  SetStringRecord(int32_t txNum, const std::shared_ptr<file::Block>& blk,
                  int32_t offset, const std::string& val);
  explicit SetStringRecord(std::unique_ptr<log::BasicLogRecord> rec);

  int32_t writeToLog() override;
  LogRecord::Type op() const override;
  int32_t txNumber() const override;
  void undo(int32_t txNum) override;

 private:
  int32_t txNum_, offset_;
  std::string val_;
  std::shared_ptr<file::Block> blk_;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql