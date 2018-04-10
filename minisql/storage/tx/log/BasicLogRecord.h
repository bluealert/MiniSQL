#pragma once

#include <cstdint>
#include <string>

namespace minisql {
namespace storage {

namespace file {
class Page;
}

namespace tx {
namespace log {

class BasicLogRecord {
 public:
  BasicLogRecord(file::Page &pg, int32_t pos);

  int32_t nextInt32();
  std::string nextString();

 private:
  file::Page &pg_;
  int32_t pos_;
};

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql