#pragma once

#include <common/Constant.h>
#include <query/scan/Scan.h>

namespace minisql {

namespace query {
class TableScan;
}

namespace storage {
namespace index {
class Index;
}
}  // namespace storage

namespace query {

class IndexSelectScan : public Scan {
 public:
  IndexSelectScan(std::unique_ptr<storage::index::Index> idx,
                  const std::shared_ptr<common::Constant>& val,
                  std::unique_ptr<TableScan> ts);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string& fldName) const override;
  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  void close() override;

 private:
  std::unique_ptr<storage::index::Index> idx_;
  std::shared_ptr<common::Constant> val_;
  std::unique_ptr<TableScan> ts_;
};

}  // namespace query
}  // namespace minisql