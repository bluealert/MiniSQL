#include <common/Int32Constant.h>
#include <query/materialize/CountFn.h>

namespace minisql {
namespace query {
namespace materialize {

CountFn::CountFn(std::string fldName) : fldName_(std::move(fldName)) {}

void CountFn::processFirst(Scan &s) { count_ = 1; }

void CountFn::processNext(Scan &s) { count_++; }

std::string CountFn::fieldName() const { return "countof" + fldName_; }

std::shared_ptr<common::Constant> CountFn::value() {
  return std::make_shared<common::Int32Constant>(count_);
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql