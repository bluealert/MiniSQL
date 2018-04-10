#include <common/Constant.h>
#include <query/ConstantExpression.h>

namespace minisql {
namespace query {

using common::Constant;

ConstantExpression::ConstantExpression(const std::shared_ptr<Constant>& c)
    : val_(c) {}

bool ConstantExpression::isConstant() { return true; }

bool ConstantExpression::isFieldName() { return false; }

std::shared_ptr<Constant> ConstantExpression::asConstant() { return val_; }

std::string ConstantExpression::asFieldName() { throw std::bad_cast(); }

std::shared_ptr<Constant> ConstantExpression::evaluate(const Scan& s) const {
  return val_;
}

bool ConstantExpression::appliesTo(const storage::record::Schema& sch) {
  return true;
}

std::string ConstantExpression::toString() { return val_->toString(); }

}  // namespace query
}  // namespace minisql