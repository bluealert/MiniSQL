#include <query/plan/ProductPlan.h>
#include <query/scan/ProductScan.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {

ProductPlan::ProductPlan(const std::shared_ptr<Plan>& p1,
                         const std::shared_ptr<Plan>& p2)
    : p1_(p1), p2_(p2) {
  sch_ = std::make_unique<storage::record::Schema>();
  sch_->addAll(*p1->schema());
  sch_->addAll(*p2->schema());
}

std::unique_ptr<Scan> ProductPlan::open() const {
  auto s1 = p1_->open();
  auto s2 = p2_->open();
  return std::make_unique<ProductScan>(std::move(s1), std::move(s2));
}

int32_t ProductPlan::blocksAccessed() const {
  return p1_->blocksAccessed() + (p1_->recordsOutput() * p2_->blocksAccessed());
}

int32_t ProductPlan::recordsOutput() const {
  return p1_->recordsOutput() * p2_->recordsOutput();
}

int32_t ProductPlan::distinctValues(const std::string& fldName) const {
  if (p1_->schema()->hasField(fldName)) {
    return p1_->distinctValues(fldName);
  } else {
    return p2_->distinctValues(fldName);
  }
}

std::shared_ptr<storage::record::Schema> ProductPlan::schema() const {
  return sch_;
}

}  // namespace query
}  // namespace minisql