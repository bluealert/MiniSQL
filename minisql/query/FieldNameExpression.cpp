#include <common/Constant.h>
#include <query/FieldNameExpression.h>
#include <query/scan/Scan.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {

using common::Constant;

FieldNameExpression::FieldNameExpression(std::string fldName)
    : fldName_(std::move(fldName)) {}

bool FieldNameExpression::isConstant() { return false; }

bool FieldNameExpression::isFieldName() { return true; }

std::shared_ptr<Constant> FieldNameExpression::asConstant() {
  throw std::bad_cast();
}

std::string FieldNameExpression::asFieldName() { return fldName_; }

std::shared_ptr<Constant> FieldNameExpression::evaluate(const Scan& s) const {
  return s.getVal(fldName_);
}

bool FieldNameExpression::appliesTo(const storage::record::Schema& sch) {
  return sch.hasField(fldName_);
}

std::string FieldNameExpression::toString() { return fldName_; }

}  // namespace query
}  // namespace minisql