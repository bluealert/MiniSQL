#pragma once

#include <query/scan/Scan.h>
#include <storage/record/RID.h>
#include <deque>
#include <vector>

namespace minisql {

namespace query {
class UpdateScan;

namespace materialize {

class TempTable;
class RecordComparator;

class SortScan : public query::Scan {
 public:
  SortScan(const std::deque<std::shared_ptr<TempTable>>& runs,
           RecordComparator& comp);

  void beforeFirst() override;
  bool next() override;
  void close() override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;
  bool hasField(const std::string& fldName) const override;

  void savePosition();
  void restorePosition();

 private:
  std::shared_ptr<query::UpdateScan> s1_, s2_, currScan_;
  RecordComparator& comp_;
  bool hasMore1_, hasMore2_;
  std::vector<std::unique_ptr<storage::record::RID>> savedPos_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql