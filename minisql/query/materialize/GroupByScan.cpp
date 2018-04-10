#include <common/Constant.h>
#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <query/materialize/AggregationFn.h>
#include <query/materialize/GroupByScan.h>

namespace minisql {
namespace query {
namespace materialize {

GroupByScan::GroupByScan(
    std::unique_ptr<Scan> s, const std::vector<std::string> &groupFields,
    const std::vector<std::shared_ptr<AggregationFn>> &aggFns)
    : s_(std::move(s)),
      groupVal_(nullptr),
      groupFields_(groupFields),
      aggFns_(aggFns) {}

void GroupByScan::beforeFirst() {
  s_->beforeFirst();
  moreGroups_ = s_->next();
}

bool GroupByScan::next() {
  if (!moreGroups_) {
    return false;
  }
  for (auto &fn : aggFns_) {
    fn->processFirst(*s_);
  }
  groupVal_ = std::make_unique<GroupValue>(*s_, groupFields_);
  while ((moreGroups_ = s_->next())) {
    GroupValue gv(*s_, groupFields_);
    if (!groupVal_->equals(gv)) {
      break;
    }
    for (auto &fn : aggFns_) {
      fn->processNext(*s_);
    }
  }
  return true;
}

void GroupByScan::close() { s_->close(); }

std::shared_ptr<common::Constant> GroupByScan::getVal(
    const std::string &fldName) const {
  if (std::find(groupFields_.begin(), groupFields_.end(), fldName) !=
      groupFields_.end()) {
    return groupVal_->getVal(fldName);
  }
  for (const auto &fn : aggFns_) {
    if (fn->fieldName() == fldName) {
      return fn->value();
    }
  }
  throw std::runtime_error("GroupByScan: field " + fldName + " not found.");
}

int32_t GroupByScan::getInt32(const std::string &fldName) const {
  return dynamic_cast<common::Int32Constant *>(getVal(fldName).get())->val();
}

std::string GroupByScan::getString(const std::string &fldName) const {
  return dynamic_cast<common::StringConstant *>(getVal(fldName).get())->val();
}

bool GroupByScan::hasField(const std::string &fldName) const {
  if (std::find(groupFields_.begin(), groupFields_.end(), fldName) !=
      groupFields_.end()) {
    return true;
  }
  for (const auto &fn : aggFns_) {
    if (fn->fieldName() == fldName) {
      return true;
    }
  }
  return false;
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql