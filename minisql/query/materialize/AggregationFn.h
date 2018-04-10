#pragma once

#include <memory>

namespace minisql {
namespace common {
class Constant;
}

namespace query {
class Scan;

namespace materialize {

class AggregationFn {
 public:
  virtual ~AggregationFn() = default;

  virtual void processFirst(query::Scan& s) = 0;
  virtual void processNext(query::Scan& s) = 0;
  virtual std::string fieldName() const = 0;
  virtual std::shared_ptr<common::Constant> value() = 0;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql