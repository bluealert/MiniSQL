#pragma once

#include <query/parse/ParseData.h>
#include <query/planner/QueryPlanner.h>
#include <deque>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
}  // namespace storage

namespace query {
namespace planner {
namespace opt {

class TablePlanner;

class HeuristicQueryPlanner : public planner::QueryPlanner {
 public:
  std::unique_ptr<query::Plan> createPlan(
      parse::QueryData &data, storage::tx::Transaction &tx) override;

 private:
  std::shared_ptr<query::Plan> getLowestSelectPlan();

  std::shared_ptr<query::Plan> getLowestJoinPlan(
      std::shared_ptr<query::Plan> curr);

  std::shared_ptr<query::Plan> getLowestProductPlan(
      std::shared_ptr<query::Plan> curr);

  void eraseTp(const std::shared_ptr<TablePlanner>& tp);

 private:
  std::deque<std::shared_ptr<TablePlanner>> tps_;
};

}  // namespace opt
}  // namespace planner
}  // namespace query
}  // namespace minisql