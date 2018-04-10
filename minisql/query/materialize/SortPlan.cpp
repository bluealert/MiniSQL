#include <query/materialize/MaterializePlan.h>
#include <query/materialize/RecordComparator.h>
#include <query/materialize/SortPlan.h>
#include <query/materialize/SortScan.h>
#include <query/materialize/TempTable.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {
namespace materialize {

SortPlan::SortPlan(const std::shared_ptr<Plan> &p,
                   const std::vector<std::string> &sortFields,
                   storage::tx::Transaction &tx)
    : p_(p), tx_(tx), sch_(p_->schema()) {
  comp_ = std::make_shared<RecordComparator>(sortFields);
}

std::unique_ptr<Scan> SortPlan::open() const {
  auto src = p_->open();
  auto runs = splitIntoRuns(*src);
  src->close();

  while (runs.size() > 2) {
    runs = doAMergeIteration(runs);
  }
  return std::make_unique<SortScan>(runs, *comp_);
}

int32_t SortPlan::blocksAccessed() const {
  MaterializePlan mp(p_, tx_);
  return mp.blocksAccessed();
}

int32_t SortPlan::recordsOutput() const { return p_->recordsOutput(); }

int32_t SortPlan::distinctValues(const std::string &fldName) const {
  return p_->distinctValues(fldName);
}

std::shared_ptr<storage::record::Schema> SortPlan::schema() const {
  return sch_;
}

std::deque<std::shared_ptr<TempTable>> SortPlan::splitIntoRuns(
    query::Scan &src) const {
  std::deque<std::shared_ptr<TempTable>> temps;
  src.beforeFirst();
  if (!src.next()) {
    return temps;
  }

  auto currTemp = std::make_shared<TempTable>(sch_, tx_);
  temps.push_back(currTemp);

  auto currScan = currTemp->open();
  while (copy(src, *currScan)) {
    if (comp_->compare(src, *currScan) < 0) {
      currTemp = std::make_shared<TempTable>(sch_, tx_);
      temps.push_back(currTemp);

      currScan->close();
      currScan = currTemp->open();
    }
  }
  currScan->close();
  return temps;
}

std::deque<std::shared_ptr<TempTable>> SortPlan::doAMergeIteration(
    std::deque<std::shared_ptr<TempTable>> &runs) const {
  std::deque<std::shared_ptr<TempTable>> result;
  while (runs.size() > 1) {
    auto t1 = std::move(runs.front());
    runs.pop_front();
    auto t2 = std::move(runs.front());
    runs.pop_front();
    result.push_back(mergeTwoRuns(t1, t2));
  }
  if (runs.size() == 1) {
    result.push_back(std::move(runs.front()));
  }
  return result;
}

std::shared_ptr<TempTable> SortPlan::mergeTwoRuns(
    const std::shared_ptr<TempTable> &t1,
    const std::shared_ptr<TempTable> &t2) const {
  auto result = std::make_shared<TempTable>(sch_, tx_);
  auto dest = result->open();

  auto src1 = t1->open();
  auto src2 = t2->open();
  bool hasMore1 = src1->next();
  bool hasMore2 = src2->next();

  while (hasMore1 && hasMore2) {
    if (comp_->compare(*src1, *src2) < 0) {
      hasMore1 = copy(*src1, *src2);
    } else {
      hasMore2 = copy(*src2, *dest);
    }
  }
  while (hasMore1) {
    hasMore1 = copy(*src1, *dest);
  }
  while (hasMore2) {
    hasMore2 = copy(*src2, *dest);
  }

  src1->close();
  src2->close();
  dest->close();

  return result;
}

bool SortPlan::copy(query::Scan &src, query::UpdateScan &dest) const {
  dest.insert();
  for (auto &fldName : sch_->fields()) {
    dest.setVal(fldName, *src.getVal(fldName));
  }
  return src.next();
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql