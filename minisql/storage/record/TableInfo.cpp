#include <storage/file/Page.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {
namespace record {

TableInfo::TableInfo(std::string tblName, const std::shared_ptr<Schema> &schema)
    : tblName_(std::move(tblName)), schema_(schema) {
  int32_t pos = 0;
  for (const auto &fldName : schema_->fields()) {
    offsets_[fldName] = pos;
    pos += lengthInBytes(fldName);
  }
  recordLen_ = pos;
}

TableInfo::TableInfo(std::string tblName, const std::shared_ptr<Schema> &schema,
                     std::unordered_map<std::string, int32_t> offsets,
                     int32_t recordLen)
    : recordLen_(recordLen),
      tblName_(std::move(tblName)),
      schema_(schema),
      offsets_(std::move(offsets)) {}

std::string TableInfo::fileName() const { return tblName_ + ".tbl"; }

std::shared_ptr<Schema> TableInfo::schema() const { return schema_; }

int32_t TableInfo::offset(const std::string &fldName) const {
  auto it = offsets_.find(fldName);
  if (it != offsets_.end()) {
    return it->second;
  }
  throw std::runtime_error("TableInfo. can't find offset by field name: " +
                           fldName);
}

int32_t TableInfo::recordLength() const { return recordLen_; }

int32_t TableInfo::lengthInBytes(const std::string &fldName) const {
  auto fldType = schema_->type(fldName);
  if (fldType == Schema::Type::INT32) {
    return file::Page::INT32_SIZE;
  } else {
    return file::Page::STR_SIZE(schema_->length(fldName));
  }
}

}  // namespace record
}  // namespace storage
}  // namespace minisql