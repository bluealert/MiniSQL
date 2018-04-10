#include <storage/file/Page.h>
#include <storage/tx/log/BasicLogRecord.h>

namespace minisql {
namespace storage {
namespace tx {
namespace log {

BasicLogRecord::BasicLogRecord(file::Page &pg, int32_t pos)
    : pg_(pg), pos_(pos) {}

int32_t BasicLogRecord::nextInt32() {
  auto result = pg_.getInt32(pos_);
  pos_ += sizeof(int32_t);
  return result;
}

std::string BasicLogRecord::nextString() {
  std::string result = pg_.getString(pos_);
  pos_ += (sizeof(int32_t) + result.length());
  return result;
}

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql