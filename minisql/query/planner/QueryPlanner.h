#pragma once

#include <query/parse/ParseData.h>
#include <query/plan/Plan.h>
#include <memory>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
}  // namespace storage

namespace query {
namespace planner {

class QueryPlanner {
 public:
  virtual ~QueryPlanner() = default;

  virtual std::unique_ptr<query::Plan> createPlan(
      parse::QueryData &data, storage::tx::Transaction &tx) = 0;
};

}  // namespace planner
}  // namespace query
}  // namespace minisql