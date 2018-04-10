#include <query/plan/IndexJoinPlan.h>
#include <query/scan/IndexJoinScan.h>
#include <query/scan/TableScan.h>
#include <storage/metadata/IndexInfo.h>

namespace minisql {
namespace query {

IndexJoinPlan::IndexJoinPlan(const std::shared_ptr<Plan> &p1,
                             const std::shared_ptr<Plan> &p2,
                             storage::metadata::IndexInfo &ii,
                             std::string joinField,
                             storage::tx::Transaction &tx)
    : joinField_(std::move(joinField)), p1_(p1), p2_(p2), ii_(ii) {
  sch_ = std::make_shared<storage::record::Schema>();
  sch_->addAll(*p1->schema());
  sch_->addAll(*p2->schema());
}

std::unique_ptr<Scan> IndexJoinPlan::open() const {
  auto s = p1_->open();
  auto ts = dynamic_cast<TableScan *>(p2_->open().get());
  auto idx = ii_.open();
  return std::make_unique<IndexJoinScan>(
      std::move(s), std::move(idx), joinField_, std::unique_ptr<TableScan>(ts));
}

int32_t IndexJoinPlan::blocksAccessed() const {
  return p1_->blocksAccessed() + (p1_->recordsOutput() * ii_.blocksAccessed()) +
         recordsOutput();
}

int32_t IndexJoinPlan::recordsOutput() const {
  return p1_->recordsOutput() * ii_.recordsOutput();
}

int32_t IndexJoinPlan::distinctValues(const std::string &fldName) const {
  if (p1_->schema()->hasField(fldName)) {
    return p1_->distinctValues(fldName);
  } else {
    return p2_->distinctValues(fldName);
  }
}

std::shared_ptr<storage::record::Schema> IndexJoinPlan::schema() const {
  return sch_;
}

}  // namespace query
}  // namespace minisql