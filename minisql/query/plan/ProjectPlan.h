#pragma once

#include <query/plan/Plan.h>
#include <vector>

namespace minisql {

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class ProjectPlan : public Plan {
 public:
  ProjectPlan(const std::shared_ptr<Plan>& p,
              const std::vector<std::string>& fieldList);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace query
}  // namespace minisql