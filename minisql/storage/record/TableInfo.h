#pragma once

#include <storage/record/Schema.h>
#include <memory>
#include <string>
#include <unordered_map>

namespace minisql {
namespace storage {
namespace record {

class Schema;

class TableInfo {
 public:
  TableInfo(std::string tblName, const std::shared_ptr<Schema>& schema);
  TableInfo(std::string tblName, const std::shared_ptr<Schema>& schema,
            std::unordered_map<std::string, int32_t> offsets,
            int32_t recordLen);

  std::string fileName() const;
  std::shared_ptr<Schema> schema() const;
  int32_t offset(const std::string& fldName) const;
  int32_t recordLength() const;

 private:
  int32_t lengthInBytes(const std::string& fldName) const;

 private:
  int32_t recordLen_;
  std::string tblName_;
  std::shared_ptr<Schema> schema_;
  std::unordered_map<std::string, int32_t> offsets_;
};

}  // namespace record
}  // namespace storage
}  // namespace minisql