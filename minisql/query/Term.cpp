#include <common/Constant.h>
#include <query/Expression.h>
#include <query/Term.h>
#include <query/plan/Plan.h>
#include <query/scan/Scan.h>

namespace minisql {
namespace query {

Term::Term(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
    : lhs_(std::move(lhs)), rhs_(std::move(rhs)) {}

int32_t Term::reductionFactor(const Plan &p) {
  std::string lhsName, rhsName;
  if (lhs_->isFieldName() && rhs_->isFieldName()) {
    lhsName = lhs_->asFieldName();
    rhsName = rhs_->asFieldName();
    return std::max(p.distinctValues(lhsName), p.distinctValues(rhsName));
  }
  if (lhs_->isFieldName()) {
    lhsName = lhs_->asFieldName();
    return p.distinctValues(lhsName);
  }
  if (rhs_->isFieldName()) {
    rhsName = rhs_->asFieldName();
    return p.distinctValues(rhsName);
  }

  if (lhs_->asConstant() == rhs_->asConstant()) {
    return 1;
  }
  return std::numeric_limits<int32_t>::max();
}

std::shared_ptr<common::Constant> Term::equatesWithConstant(
    const std::string &fldName) {
  if (lhs_->isFieldName() && lhs_->asFieldName() == fldName &&
      rhs_->isConstant()) {
    return rhs_->asConstant();
  } else if (rhs_->isFieldName() && rhs_->asFieldName() == fldName &&
             lhs_->isConstant()) {
    return lhs_->asConstant();
  } else {
    return nullptr;
  }
}

std::string Term::equatesWithField(const std::string &fldName) {
  if (lhs_->isFieldName() && lhs_->asFieldName() == fldName &&
      rhs_->isFieldName()) {
    return rhs_->asFieldName();
  } else if (rhs_->isFieldName() && rhs_->asFieldName() == fldName &&
             lhs_->isFieldName()) {
    return lhs_->asFieldName();
  } else {
    return "";
  }
}

bool Term::appliesTo(const storage::record::Schema &sch) {
  return lhs_->appliesTo(sch) && rhs_->appliesTo(sch);
}

bool Term::isSatisfied(const Scan &s) {
  auto lhsVal = lhs_->evaluate(s);
  auto rhsVal = rhs_->evaluate(s);
  return lhsVal->equals(*rhsVal);
}

std::string Term::toString() {
  return lhs_->toString() + " = " + rhs_->toString();
}

}  // namespace query
}  // namespace minisql