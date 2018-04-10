#pragma once

#include <query/Expression.h>
#include <memory>

namespace minisql {

namespace common {
class Constant;
}

namespace query {

class Scan;

class ConstantExpression : public Expression {
 public:
  explicit ConstantExpression(const std::shared_ptr<common::Constant>& c);

  bool isConstant() override;
  bool isFieldName() override;
  std::shared_ptr<common::Constant> asConstant() override;
  std::string asFieldName() override;
  std::shared_ptr<common::Constant> evaluate(const Scan& s) const override;
  bool appliesTo(const storage::record::Schema& sch) override;
  std::string toString() override;

 private:
  std::shared_ptr<common::Constant> val_;
};

}  // namespace query
}  // namespace minisql