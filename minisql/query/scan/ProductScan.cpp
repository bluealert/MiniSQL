#include <common/Constant.h>
#include <query/scan/ProductScan.h>

namespace minisql {
namespace query {

ProductScan::ProductScan(std::unique_ptr<query::Scan> s1,
                         std::unique_ptr<query::Scan> s2)
    : s1_(std::move(s1)), s2_(std::move(s2)) {
  s1_->next();
}

void ProductScan::beforeFirst() {
  s1_->beforeFirst();
  s1_->next();
  s2_->beforeFirst();
}

bool ProductScan::next() {
  if (s2_->next()) {
    return true;
  } else {
    s2_->beforeFirst();
    return s2_->next() && s1_->next();
  }
}

void ProductScan::close() {
  s1_->close();
  s2_->close();
}

std::shared_ptr<common::Constant> ProductScan::getVal(
    const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getVal(fldName);
  } else {
    return s2_->getVal(fldName);
  }
}

int32_t ProductScan::getInt32(const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getInt32(fldName);
  } else {
    return s2_->getInt32(fldName);
  }
}

std::string ProductScan::getString(const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getString(fldName);
  } else {
    return s2_->getString(fldName);
  }
}

bool ProductScan::hasField(const std::string &fldName) const {
  return s1_->hasField(fldName) || s2_->hasField(fldName);
}

}  // namespace query
}  // namespace minisql