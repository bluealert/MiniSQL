#pragma once

#include <memory>

namespace minisql {
namespace common {
class Constant;
}

namespace storage {
namespace record {
class Schema;
}
}  // namespace storage

namespace query {

class Scan;

class Expression {
 public:
  virtual ~Expression() = default;

  virtual bool isConstant() = 0;
  virtual bool isFieldName() = 0;
  virtual std::shared_ptr<common::Constant> asConstant() = 0;
  virtual std::string asFieldName() = 0;
  virtual std::shared_ptr<common::Constant> evaluate(const Scan& s) const = 0;
  virtual bool appliesTo(const storage::record::Schema& sch) = 0;
  virtual std::string toString() = 0;
};

}  // namespace query
}  // namespace minisql