#pragma once

#include <query/parse/ParseData.h>
#include <query/planner/QueryPlanner.h>

namespace minisql {
namespace query {
namespace planner {
namespace basic {

class BasicQueryPlanner : public QueryPlanner {
 public:
  std::unique_ptr<query::Plan> createPlan(
      parse::QueryData &data, storage::tx::Transaction &tx) override;
};

}  // namespace basic
}  // namespace planner
}  // namespace query
}  // namespace minisql