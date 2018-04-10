#pragma once

#include <memory>
#include <string>
#include <unordered_map>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
namespace record {
class Schema;
}
namespace metadata {
class IndexInfo;
}
}  // namespace storage

namespace query {

class Plan;
class TablePlan;
class Predicate;

namespace planner {
namespace opt {

class TablePlanner {
 public:
  TablePlanner(const std::string &tblName,
               const std::shared_ptr<query::Predicate> &myPred,
               storage::tx::Transaction &tx);

  std::shared_ptr<query::Plan> makeSelectPlan();

  std::shared_ptr<query::Plan> makeJoinPlan(
      const std::shared_ptr<query::Plan> &curr);

  std::shared_ptr<query::Plan> makeProductPlan(
      const std::shared_ptr<query::Plan> &curr);

 private:
  std::shared_ptr<query::Plan> makeIndexSelect();

  std::shared_ptr<query::Plan> makeIndexJoin(
      const std::shared_ptr<query::Plan> &curr,
      const storage::record::Schema &currSch);

  std::shared_ptr<query::Plan> makeProductJoin(
      const std::shared_ptr<query::Plan> &curr,
      const storage::record::Schema &currSch);

  std::shared_ptr<query::Plan> addSelectPred(
      const std::shared_ptr<query::Plan> &p);

  std::shared_ptr<query::Plan> addJoinPred(
      const std::shared_ptr<query::Plan> &p,
      const storage::record::Schema &currSch);

 private:
  storage::tx::Transaction &tx_;
  std::shared_ptr<query::TablePlan> myPlan_;
  std::shared_ptr<query::Predicate> myPred_;
  std::shared_ptr<storage::record::Schema> mySch_;
  std::unordered_map<std::string, std::shared_ptr<storage::metadata::IndexInfo>>
      indexes_;
};

}  // namespace opt
}  // namespace planner
}  // namespace query
}  // namespace minisql