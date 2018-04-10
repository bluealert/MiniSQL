#pragma once

#include <query/scan/UpdateScan.h>
#include <storage/record/RID.h>
#include <storage/record/RecordFile.h>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class Schema;
class TableInfo;
}  // namespace record
}  // namespace storage

namespace query {

class TableScan : public UpdateScan {
 public:
  TableScan(const std::shared_ptr<storage::record::TableInfo>& ti,
            storage::tx::Transaction& tx);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string& fldName) const override;
  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  void close() override;

  void insert() override;
  void setVal(const std::string& fldName, const common::Constant& val) override;
  void setInt32(const std::string& fldName, int32_t val) override;
  void setString(const std::string& fldName, const std::string& val) override;

  void del() override;

  std::unique_ptr<storage::record::RID> getRid() const override;
  void moveToRid(const storage::record::RID& rid) override;

 private:
  storage::record::RecordFile rf_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace query
}  // namespace minisql