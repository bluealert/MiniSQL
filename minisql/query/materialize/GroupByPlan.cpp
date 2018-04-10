#include <query/materialize/AggregationFn.h>
#include <query/materialize/GroupByPlan.h>
#include <query/materialize/GroupByScan.h>
#include <query/materialize/SortPlan.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {
namespace materialize {

GroupByPlan::GroupByPlan(const std::shared_ptr<Plan> &p,
                         std::vector<std::string> groupFields,
                         std::vector<std::shared_ptr<AggregationFn>> aggFns,
                         storage::tx::Transaction &tx)
    : groupFields_(std::move(groupFields)), aggFns_(std::move(aggFns)) {
  p_ = std::make_shared<SortPlan>(p, groupFields_, tx);

  sch_ = std::make_shared<storage::record::Schema>();
  for (const auto &fldName : groupFields_) {
    sch_->add(fldName, *p->schema());
  }
  for (const auto &fn : aggFns_) {
    sch_->addInt32Field(fn->fieldName());
  }
}

std::unique_ptr<query::Scan> GroupByPlan::open() const {
  auto s = p_->open();
  return std::make_unique<GroupByScan>(std::move(s), groupFields_, aggFns_);
}

int32_t GroupByPlan::blocksAccessed() const { return p_->blocksAccessed(); }

int32_t GroupByPlan::recordsOutput() const {
  auto numGroups = 1;
  for (auto &fldName : groupFields_) {
    numGroups *= p_->distinctValues(fldName);
  }
  return numGroups;
}

int32_t GroupByPlan::distinctValues(const std::string &fldName) const {
  if (p_->schema()->hasField(fldName)) {
    return p_->distinctValues(fldName);
  } else {
    return recordsOutput();
  }
}

std::shared_ptr<storage::record::Schema> GroupByPlan::schema() const {
  return sch_;
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql