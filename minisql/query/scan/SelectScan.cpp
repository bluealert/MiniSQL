#include <common/Constant.h>
#include <query/Expression.h>
#include <query/Predicate.h>
#include <query/Term.h>
#include <query/scan/SelectScan.h>

namespace minisql {
namespace query {

SelectScan::SelectScan(std::unique_ptr<Scan> s,
                       const std::shared_ptr<Predicate> &pred)
    : s_(std::move(s)), pred_(pred) {}

void SelectScan::beforeFirst() { s_->beforeFirst(); }

bool SelectScan::next() {
  while (s_->next()) {
    if (pred_ == nullptr) {
      return true;
    }
    if (pred_->isSatisfied(*s_)) {
      return true;
    }
  }
  return false;
}

void SelectScan::close() { s_->close(); }

std::shared_ptr<common::Constant> SelectScan::getVal(
    const std::string &fldName) const {
  return s_->getVal(fldName);
}

int32_t SelectScan::getInt32(const std::string &fldName) const {
  return s_->getInt32(fldName);
}

std::string SelectScan::getString(const std::string &fldName) const {
  return s_->getString(fldName);
}

bool SelectScan::hasField(const std::string &fldName) const {
  return s_->hasField(fldName);
}

void SelectScan::setVal(const std::string &fldName,
                        const common::Constant &val) {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->setVal(fldName, val);
}

void SelectScan::setInt32(const std::string &fldName, int32_t val) {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->setInt32(fldName, val);
}

void SelectScan::setString(const std::string &fldName, const std::string &val) {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->setString(fldName, val);
}

void SelectScan::insert() {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->insert();
}

void SelectScan::del() {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->del();
}

std::unique_ptr<storage::record::RID> SelectScan::getRid() const {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  return us->getRid();
}

void SelectScan::moveToRid(const storage::record::RID &rid) {
  auto us = dynamic_cast<UpdateScan *>(s_.get());
  us->moveToRid(rid);
}

}  // namespace query
}  // namespace minisql