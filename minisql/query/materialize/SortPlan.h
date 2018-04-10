#pragma once

#include <query/plan/Plan.h>
#include <deque>
#include <vector>

namespace minisql {

namespace storage {
namespace record {
class Schema;
}
namespace tx {
class Transaction;
}
}  // namespace storage

namespace query {
class UpdateScan;

namespace materialize {

class TempTable;
class RecordComparator;

class SortPlan : public Plan {
 public:
  SortPlan(const std::shared_ptr<Plan>& p,
           const std::vector<std::string>& sortFields,
           storage::tx::Transaction& tx);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::deque<std::shared_ptr<TempTable>> splitIntoRuns(Scan& src) const;

  std::deque<std::shared_ptr<TempTable>> doAMergeIteration(
      std::deque<std::shared_ptr<TempTable>>& runs) const;

  std::shared_ptr<TempTable> mergeTwoRuns(
      const std::shared_ptr<TempTable>& t1,
      const std::shared_ptr<TempTable>& t2) const;

  bool copy(Scan& src, UpdateScan& dest) const;

 private:
  std::shared_ptr<Plan> p_;
  storage::tx::Transaction& tx_;
  std::shared_ptr<RecordComparator> comp_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql