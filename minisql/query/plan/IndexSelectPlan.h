#pragma once

#include <query/plan/Plan.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace common {
class Constant;
}

namespace storage {
namespace record {
class Schema;
}
namespace metadata {
class IndexInfo;
}
}  // namespace storage

namespace query {

class IndexSelectPlan : public Plan {
 public:
  IndexSelectPlan(const std::shared_ptr<Plan>& p,
                  storage::metadata::IndexInfo& ii,
                  const std::shared_ptr<common::Constant>& val);

  std::unique_ptr<Scan> open() const override;
  int32_t blocksAccessed() const override;
  int32_t recordsOutput() const override;
  int32_t distinctValues(const std::string& fldName) const override;
  std::shared_ptr<storage::record::Schema> schema() const override;

 private:
  std::shared_ptr<Plan> p_;
  storage::metadata::IndexInfo& ii_;
  std::shared_ptr<common::Constant> val_;
};

}  // namespace query
}  // namespace minisql