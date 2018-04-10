#pragma once

#include <storage/buffer/PageFormatter.h>
#include <memory>

namespace minisql {
namespace storage {

namespace file {
class Page;
}
namespace record {
class TableInfo;
}

namespace index {
namespace btree {

class BTPageFormatter : public storage::buffer::PageFormatter {
 public:
  BTPageFormatter(const storage::record::TableInfo &ti, int32_t flag);

  void format(storage::file::Page &p) override;

 private:
  void makeDefaultRecord(storage::file::Page &p, int32_t pos);

 private:
  const storage::record::TableInfo &ti_;
  int32_t flag_;
};

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql