#pragma once

#include <query/plan/Plan.h>

namespace minisql {

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class ProductPlan : public Plan {
 public:
  ProductPlan(const std::shared_ptr<Plan>& p1, const std::shared_ptr<Plan>& p2);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p1_, p2_;
  std::shared_ptr<storage::record::Schema> sch_;
};

}  // namespace query
}  // namespace minisql