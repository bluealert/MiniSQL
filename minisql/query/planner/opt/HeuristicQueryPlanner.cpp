#include <query/plan/ProjectPlan.h>
#include <query/planner/opt/HeuristicQueryPlanner.h>
#include <query/planner/opt/TablePlanner.h>

namespace minisql {
namespace query {
namespace planner {
namespace opt {

std::unique_ptr<query::Plan> HeuristicQueryPlanner::createPlan(
    parse::QueryData &data, storage::tx::Transaction &tx) {
  for (auto &tblName : data.tables()) {
    auto tp = std::make_shared<TablePlanner>(tblName, data.pred(), tx);
    tps_.push_back(tp);
  }

  auto currPlan = getLowestSelectPlan();

  while (!tps_.empty()) {
    auto p = getLowestJoinPlan(currPlan);
    if (p != nullptr) {
      currPlan = p;
    } else {
      currPlan = getLowestProductPlan(currPlan);
    }
  }

  return std::make_unique<query::ProjectPlan>(currPlan, data.fields());
}

std::shared_ptr<query::Plan> HeuristicQueryPlanner::getLowestSelectPlan() {
  std::shared_ptr<TablePlanner> bestTp = nullptr;
  std::shared_ptr<query::Plan> bestPlan = nullptr;
  for (auto &tp : tps_) {
    auto plan = tp->makeSelectPlan();
    if (bestPlan == nullptr ||
        plan->recordsOutput() < bestPlan->recordsOutput()) {
      bestTp = tp;
      bestPlan = plan;
    }
  }
  eraseTp(bestTp);
  return bestPlan;
}

std::shared_ptr<query::Plan> HeuristicQueryPlanner::getLowestJoinPlan(
    std::shared_ptr<query::Plan> curr) {
  std::shared_ptr<TablePlanner> bestTp = nullptr;
  std::shared_ptr<query::Plan> bestPlan = nullptr;
  for (auto &tp : tps_) {
    auto plan = tp->makeJoinPlan(curr);
    if (plan != nullptr &&
        (bestPlan == nullptr ||
         plan->recordsOutput() < bestPlan->recordsOutput())) {
      bestTp = tp;
      bestPlan = plan;
    }
  }
  if (bestPlan != nullptr) {
    eraseTp(bestTp);
  }
  return bestPlan;
}

std::shared_ptr<query::Plan> HeuristicQueryPlanner::getLowestProductPlan(
    std::shared_ptr<query::Plan> curr) {
  std::shared_ptr<TablePlanner> bestTp = nullptr;
  std::shared_ptr<query::Plan> bestPlan = nullptr;
  for (auto &tp : tps_) {
    auto plan = tp->makeProductPlan(curr);
    if (bestPlan == nullptr ||
        plan->recordsOutput() < bestPlan->recordsOutput()) {
      bestTp = tp;
      bestPlan = plan;
    }
  }
  eraseTp(bestTp);
  return bestPlan;
}

void HeuristicQueryPlanner::eraseTp(std::shared_ptr<TablePlanner> tp) {
  auto it = std::find(tps_.begin(), tps_.end(), tp);
  if (it != tps_.end()) {
    tps_.erase(it);
  }
}

}  // namespace opt
}  // namespace planner
}  // namespace query
}  // namespace minisql