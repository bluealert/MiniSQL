#pragma once

#include <cstdint>

namespace minisql {
namespace storage {
namespace tx {
namespace recovery {

class LogRecord {
 public:
  enum class Type {
    CHECKPOINT = 0,
    START = 1,
    COMMIT = 2,
    ROLLBACK = 3,
    SETINT32 = 4,
    SETSTRING = 5
  };

  virtual ~LogRecord() = default;

  virtual int32_t writeToLog() = 0;
  virtual LogRecord::Type op() const = 0;
  virtual int32_t txNumber() const = 0;
  virtual void undo(int32_t txNum) = 0;
};

}  // namespace recovery
}  // namespace tx
}  // namespace storage
}  // namespace minisql