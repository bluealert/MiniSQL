#pragma once

#include <query/planner/QueryPlanner.h>
#include <query/planner/UpdatePlanner.h>

namespace minisql {
namespace query {
namespace planner {

class Planner {
 public:
  Planner(std::unique_ptr<QueryPlanner> qp, std::unique_ptr<UpdatePlanner> up);

  std::unique_ptr<query::Plan> createQueryPlan(const std::string &qry,
                                               storage::tx::Transaction &tx);

  int32_t executeUpdate(const std::string &cmd, storage::tx::Transaction &tx);

 private:
  std::unique_ptr<QueryPlanner> qp_;
  std::unique_ptr<UpdatePlanner> up_;
};

}  // namespace planner
}  // namespace query
}  // namespace minisql