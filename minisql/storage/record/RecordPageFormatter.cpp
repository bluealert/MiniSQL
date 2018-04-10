#include <storage/file/Page.h>
#include <storage/record/RecordPage.h>
#include <storage/record/RecordPageFormatter.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {
namespace record {

RecordPageFormatter::RecordPageFormatter(const std::shared_ptr<TableInfo> &ti)
    : ti_(ti) {}

void RecordPageFormatter::format(file::Page &page) {
  auto recSize = ti_->recordLength() + file::Page::INT32_SIZE;
  for (int32_t pos = 0; pos + recSize <= file::Page::BLOCK_SIZE;
       pos += recSize) {
    page.setInt32(pos, record::RecordPage::EMPTY);

    for (const auto &fldName : ti_->schema()->fields()) {
      auto offset = ti_->offset(fldName);
      if (ti_->schema()->type(fldName) == Schema::Type::INT32) {
        page.setInt32(pos + file::Page::INT32_SIZE + offset, 0);
      } else {
        page.setString(pos + file::Page::INT32_SIZE + offset, "");
      }
    }
  }
}

}  // namespace record
}  // namespace storage
}  // namespace minisql