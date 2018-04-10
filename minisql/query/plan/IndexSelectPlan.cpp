#include <query/plan/IndexSelectPlan.h>
#include <query/scan/IndexSelectScan.h>
#include <query/scan/TableScan.h>
#include <storage/metadata/IndexInfo.h>
#include <storage/record/RecordFile.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {

IndexSelectPlan::IndexSelectPlan(const std::shared_ptr<Plan>& p,
                                 storage::metadata::IndexInfo& ii,
                                 const std::shared_ptr<common::Constant>& val)
    : p_(p), ii_(ii), val_(val) {}

std::unique_ptr<Scan> IndexSelectPlan::open() const {
  auto ts = dynamic_cast<TableScan*>(p_->open().get());
  auto idx = ii_.open();
  return std::make_unique<IndexSelectScan>(std::move(idx), val_,
                                           std::unique_ptr<TableScan>(ts));
}

int32_t IndexSelectPlan::blocksAccessed() const {
  return ii_.blocksAccessed() + recordsOutput();
}

int32_t IndexSelectPlan::recordsOutput() const { return ii_.recordsOutput(); }

int32_t IndexSelectPlan::distinctValues(const std::string& fldName) const {
  return ii_.distinctValues(fldName);
}

std::shared_ptr<storage::record::Schema> IndexSelectPlan::schema() const {
  return p_->schema();
}

}  // namespace query
}  // namespace minisql