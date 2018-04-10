#pragma once

#include <query/scan/UpdateScan.h>

namespace minisql {
namespace query {

class Scan;
class Predicate;

class SelectScan : public UpdateScan {
 public:
  SelectScan(std::unique_ptr<Scan> s, const std::shared_ptr<Predicate>& pred);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string& fldName) const override;
  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  void close() override;

  void insert() override;
  void setInt32(const std::string& fldName, int32_t val) override;
  void setString(const std::string& fldName, const std::string& val) override;
  void setVal(const std::string& fldName, const common::Constant& val) override;

  void del() override;

  std::unique_ptr<storage::record::RID> getRid() const override;
  void moveToRid(const storage::record::RID& rid) override;

 private:
  std::unique_ptr<Scan> s_;
  std::shared_ptr<Predicate> pred_;
};

}  // namespace query
}  // namespace minisql