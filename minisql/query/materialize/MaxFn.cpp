#include <common/Constant.h>
#include <query/materialize/MaxFn.h>
#include <query/scan/Scan.h>

namespace minisql {
namespace query {
namespace materialize {

MaxFn::MaxFn(const std::string &fldName) : fldName_(fldName) {}

void MaxFn::processFirst(Scan &s) { val_ = s.getVal(fldName_); }

void MaxFn::processNext(Scan &s) {
  auto newVal = s.getVal(fldName_);
  if (val_->compareTo(*newVal) > 0) {
    val_ = newVal;
  }
}

std::string MaxFn::fieldName() const { return "maxof" + fldName_; }

std::shared_ptr<common::Constant> MaxFn::value() { return val_; }

}  // namespace materialize
}  // namespace query
}  // namespace minisql