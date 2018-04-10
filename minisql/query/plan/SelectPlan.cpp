#include <query/Expression.h>
#include <query/Term.h>
#include <query/plan/SelectPlan.h>
#include <query/scan/SelectScan.h>

namespace minisql {
namespace query {

SelectPlan::SelectPlan(const std::shared_ptr<query::Plan>& p,
                       const std::shared_ptr<query::Predicate>& pred)
    : p_(p), pred_(pred) {}

std::unique_ptr<Scan> SelectPlan::open() const {
  return std::make_unique<SelectScan>(p_->open(), pred_);
}

int32_t SelectPlan::blocksAccessed() const { return p_->blocksAccessed(); }

int32_t SelectPlan::recordsOutput() const {
  return p_->recordsOutput() / pred_->reductionFactor(*p_);
}

int32_t SelectPlan::distinctValues(const std::string& fldName) const {
  if (pred_->equatesWithConstant(fldName) != nullptr) {
    return 1;
  }

  auto fldName2 = pred_->equatesWithField(fldName);
  if (fldName2.empty()) {
    return std::min(p_->distinctValues(fldName), recordsOutput());
  }

  return std::min(p_->distinctValues(fldName), p_->distinctValues(fldName2));
}

std::shared_ptr<storage::record::Schema> SelectPlan::schema() const {
  return p_->schema();
}

}  // namespace query
}  // namespace minisql