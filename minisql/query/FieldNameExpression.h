#pragma once

#include <query/Expression.h>
#include <string>

namespace minisql {

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class Scan;

class FieldNameExpression : public Expression {
 public:
  explicit FieldNameExpression(std::string fldName);

  bool isConstant() override;
  bool isFieldName() override;
  std::shared_ptr<common::Constant> asConstant() override;
  std::string asFieldName() override;
  std::shared_ptr<common::Constant> evaluate(const Scan& s) const override;
  bool appliesTo(const storage::record::Schema& sch) override;
  std::string toString() override;

 private:
  std::string fldName_;
};

}  // namespace query

}  // namespace minisql