#pragma once

#include <storage/buffer/PageFormatter.h>
#include <memory>

namespace minisql {
namespace storage {
namespace record {

class TableInfo;

class RecordPageFormatter : public buffer::PageFormatter {
 public:
  explicit RecordPageFormatter(const std::shared_ptr<TableInfo>& ti);

  void format(file::Page& page) override;

 private:
  std::shared_ptr<TableInfo> ti_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql