#pragma once

#include <query/materialize/AggregationFn.h>
#include <string>

namespace minisql {
namespace common {
class Constant;
}

namespace query {
class Scan;

namespace materialize {

class CountFn : public AggregationFn {
 public:
  explicit CountFn(std::string fldName);

  void processFirst(Scan &s) override;
  void processNext(Scan &s) override;
  std::string fieldName() const override;
  std::shared_ptr<common::Constant> value() override;

 private:
  int32_t count_;
  std::string fldName_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql