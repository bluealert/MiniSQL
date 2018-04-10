#include <query/materialize/MergeJoinPlan.h>
#include <query/materialize/MergeJoinScan.h>
#include <query/materialize/SortPlan.h>
#include <query/materialize/SortScan.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {
namespace materialize {

MergeJoinPlan::MergeJoinPlan(const std::shared_ptr<Plan> &p1,
                             const std::shared_ptr<Plan> &p2,
                             const std::string &fldName1,
                             const std::string &fldName2,
                             storage::tx::Transaction &tx)
    : fldName1_(fldName1), fldName2_(fldName2) {
  std::vector<std::string> sortList1{fldName1};
  p1_ = std::make_shared<SortPlan>(p1, sortList1, tx);

  std::vector<std::string> sortList2{fldName2};
  p2_ = std::make_shared<SortPlan>(p2, sortList2, tx);

  sch_ = std::make_unique<storage::record::Schema>();
  sch_->addAll(*p1->schema());
  sch_->addAll(*p2->schema());
}

std::unique_ptr<Scan> MergeJoinPlan::open() const {
  auto s1 = p1_->open();
  auto s2 =
      std::unique_ptr<SortScan>(dynamic_cast<SortScan *>(p2_->open().get()));
  return std::make_unique<MergeJoinScan>(std::move(s1), std::move(s2),
                                         fldName1_, fldName2_);
}

int32_t MergeJoinPlan::blocksAccessed() const {
  return p1_->blocksAccessed() + p2_->blocksAccessed();
}

int32_t MergeJoinPlan::recordsOutput() const {
  int32_t maxVals =
      std::max(p1_->distinctValues(fldName1_), p2_->distinctValues(fldName2_));
  return (p1_->recordsOutput() * p2_->recordsOutput()) / maxVals;
}

int32_t MergeJoinPlan::distinctValues(const std::string &fldName) const {
  if (p1_->schema()->hasField(fldName)) {
    return p1_->distinctValues(fldName);
  } else {
    return p2_->distinctValues(fldName);
  }
}

std::shared_ptr<storage::record::Schema> MergeJoinPlan::schema() const {
  return sch_;
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql