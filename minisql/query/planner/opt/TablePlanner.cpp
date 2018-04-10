#include <query/Expression.h>
#include <query/Term.h>
#include <query/multibuffer/MultiBufferProductPlan.h>
#include <query/plan/IndexJoinPlan.h>
#include <query/plan/IndexSelectPlan.h>
#include <query/plan/SelectPlan.h>
#include <query/plan/TablePlan.h>
#include <query/planner/opt/TablePlanner.h>
#include <storage/metadata/MetadataMgr.h>

namespace minisql {
namespace query {
namespace planner {
namespace opt {

TablePlanner::TablePlanner(const std::string &tblName,
                           const std::shared_ptr<Predicate> &myPred,
                           storage::tx::Transaction &tx)
    : tx_(tx),
      myPlan_(std::make_shared<TablePlan>(tblName, tx)),
      myPred_(myPred),
      mySch_(myPlan_->schema()) {
  indexes_ = storage::metadata::MetadataMgr::get().getIndexInfo(tblName, tx);
}

std::shared_ptr<Plan> TablePlanner::makeSelectPlan() {
  auto p = makeIndexSelect();
  if (p == nullptr) {
    p = myPlan_;
  }
  return addSelectPred(p);
}

std::shared_ptr<Plan> TablePlanner::makeJoinPlan(
    const std::shared_ptr<Plan> &curr) {
  auto currSch = curr->schema();
  auto joinPred = myPred_->joinPred(*mySch_, *currSch);
  if (joinPred == nullptr) {
    return nullptr;
  }
  auto p = makeIndexJoin(curr, *currSch);
  if (p == nullptr) {
    p = makeProductJoin(curr, *currSch);
  }
  return p;
}

std::shared_ptr<Plan> TablePlanner::makeProductPlan(
    const std::shared_ptr<Plan> &curr) {
  auto p = addSelectPred(myPlan_);
  return std::make_shared<multibuffer::MultiBufferProductPlan>(curr, p, tx_);
}

std::shared_ptr<Plan> TablePlanner::makeIndexSelect() {
  for (auto &kv : indexes_) {
    auto val = myPred_->equatesWithConstant(kv.first);
    if (val != nullptr) {
      auto ii = indexes_[kv.first];
      return std::make_shared<IndexSelectPlan>(myPlan_, *ii, val);
    }
  }
  return nullptr;
}

std::shared_ptr<Plan> TablePlanner::makeIndexJoin(
    const std::shared_ptr<Plan> &curr, const storage::record::Schema &currSch) {
  for (auto &kv : indexes_) {
    auto outerField = myPred_->equatesWithField(kv.first);
    if (!outerField.empty() && currSch.hasField(outerField)) {
      auto ii = indexes_[kv.first];
      auto p1 =
          std::make_shared<IndexJoinPlan>(curr, myPlan_, *ii, outerField, tx_);
      auto p2 = addSelectPred(p1);
      return addJoinPred(p2, currSch);
    }
  }
  return nullptr;
}

std::shared_ptr<Plan> TablePlanner::makeProductJoin(
    const std::shared_ptr<Plan> &curr, const storage::record::Schema &currSch) {
  auto p = makeProductPlan(curr);
  return addJoinPred(p, currSch);
}

std::shared_ptr<Plan> TablePlanner::addSelectPred(
    const std::shared_ptr<Plan> &p) {
  auto selectPred = myPred_->selectPred(*mySch_);
  if (selectPred != nullptr) {
    return std::make_shared<SelectPlan>(p, selectPred);
  } else {
    return p;
  }
}

std::shared_ptr<Plan> TablePlanner::addJoinPred(
    const std::shared_ptr<Plan> &p, const storage::record::Schema &currSch) {
  auto joinPred = myPred_->joinPred(currSch, *mySch_);
  if (joinPred != nullptr) {
    return std::make_shared<SelectPlan>(p, joinPred);
  } else {
    return p;
  }
}

}  // namespace opt
}  // namespace planner
}  // namespace query
}  // namespace minisql