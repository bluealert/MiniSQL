#include <query/plan/ProjectPlan.h>
#include <query/scan/ProjectScan.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {

ProjectPlan::ProjectPlan(const std::shared_ptr<Plan>& p,
                         const std::vector<std::string>& fieldList)
    : p_(p) {
  sch_ = std::make_unique<storage::record::Schema>();
  for (auto& fldName : fieldList) {
    sch_->add(fldName, *p->schema());
  }
}

std::unique_ptr<Scan> ProjectPlan::open() const {
  auto s = p_->open();
  return std::make_unique<ProjectScan>(std::move(s), sch_->fields());
}

int32_t ProjectPlan::blocksAccessed() const { return p_->blocksAccessed(); }

int32_t ProjectPlan::recordsOutput() const { return p_->recordsOutput(); }

int32_t ProjectPlan::distinctValues(const std::string& fldName) const {
  return p_->distinctValues(fldName);
}

std::shared_ptr<storage::record::Schema> ProjectPlan::schema() const {
  return sch_;
}

}  // namespace query
}  // namespace minisql