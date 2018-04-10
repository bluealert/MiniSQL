#pragma once

#include <query/materialize/AggregationFn.h>
#include <string>

namespace minisql {
namespace common {
class Constant;
}

namespace query {
namespace materialize {

class MaxFn : public AggregationFn {
 public:
  explicit MaxFn(const std::string &fldName);

  void processFirst(Scan &s) override;
  void processNext(Scan &s) override;
  std::string fieldName() const override;
  std::shared_ptr<common::Constant> value() override;

 private:
  std::string fldName_;
  std::shared_ptr<common::Constant> val_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql