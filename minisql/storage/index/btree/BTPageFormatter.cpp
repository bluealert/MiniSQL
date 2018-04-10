#include <storage/file/Page.h>
#include <storage/index/btree/BTPageFormatter.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

BTPageFormatter::BTPageFormatter(const storage::record::TableInfo &ti,
                                 int32_t flag)
    : ti_(ti), flag_(flag) {}

void BTPageFormatter::format(storage::file::Page &p) {
  p.setInt32(0, flag_);
  p.setInt32(storage::file::Page::INT32_SIZE, 0);
  auto recSize = ti_.recordLength();
  for (int32_t pos = 2 * storage::file::Page::INT32_SIZE;
       pos + recSize <= storage::file::Page::BLOCK_SIZE; pos += recSize) {
    makeDefaultRecord(p, pos);
  }
}

void BTPageFormatter::makeDefaultRecord(storage::file::Page &p, int32_t pos) {
  for (auto &fldName : ti_.schema()->fields()) {
    auto offset = ti_.offset(fldName);
    if (ti_.schema()->type(fldName) == storage::record::Schema::Type::INT32) {
      p.setInt32(pos + offset, 0);
    } else {
      p.setString(pos + offset, "");
    }
  }
}

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql