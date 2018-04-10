#include <init/MiniSQLInit.h>
#include <query/plan/ProductPlan.h>
#include <query/plan/ProjectPlan.h>
#include <query/plan/SelectPlan.h>
#include <query/plan/TablePlan.h>
#include <query/planner/basic/BasicQueryPlanner.h>
#include <storage/metadata/MetadataMgr.h>
#include <deque>

namespace minisql {
namespace query {
namespace planner {
namespace basic {

std::unique_ptr<Plan> BasicQueryPlanner::createPlan(
    parse::QueryData &data, storage::tx::Transaction &tx) {
  // step 1: create a plan for each mentioned table or view
  std::deque<std::shared_ptr<Plan>> plans;
  for (auto &tblName : data.tables()) {
    auto viewDef =
        storage::metadata::MetadataMgr::get().getViewDef(tblName, tx);
    if (!viewDef.empty()) {
      plans.push_back(
          init::MiniSQLInit::planner()->createQueryPlan(viewDef, tx));
    } else {
      plans.push_back(std::make_shared<TablePlan>(tblName, tx));
    }
  }

  if (plans.empty()) {
    return nullptr;
  }

  // step 2: create the product of all table plans
  std::shared_ptr<Plan> p{plans[0]};
  plans.pop_front();
  for (auto &nextPlan : plans) {
    p = std::make_shared<ProductPlan>(p, nextPlan);
  }

  // step 3: add a selection plan for the predicate
  p = std::make_shared<SelectPlan>(p, data.pred());

  // step 4: project on the field names
  return std::make_unique<ProjectPlan>(p, data.fields());
}

}  // namespace basic
}  // namespace planner
}  // namespace query
}  // namespace minisql