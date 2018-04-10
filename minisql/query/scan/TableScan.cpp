#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <query/scan/TableScan.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace query {

using namespace storage;
using common::Constant;
using common::Int32Constant;
using common::StringConstant;

TableScan::TableScan(const std::shared_ptr<record::TableInfo> &ti,
                     tx::Transaction &tx)
    : rf_(ti, tx), sch_(ti->schema()) {}

void TableScan::beforeFirst() { rf_.beforeFirst(); }

bool TableScan::next() { return rf_.next(); }

void TableScan::close() { rf_.close(); }

std::shared_ptr<Constant> TableScan::getVal(const std::string &fldName) const {
  if (sch_->type(fldName) == record::Schema::Type::INT32) {
    return std::make_shared<Int32Constant>(rf_.getInt32(fldName));
  } else {
    return std::make_shared<StringConstant>(rf_.getString(fldName));
  }
}

int32_t TableScan::getInt32(const std::string &fldName) const {
  return rf_.getInt32(fldName);
}

std::string TableScan::getString(const std::string &fldName) const {
  return rf_.getString(fldName);
}

bool TableScan::hasField(const std::string &fldName) const {
  return sch_->hasField(fldName);
}

void TableScan::setVal(const std::string &fldName, const Constant &val) {
  if (val.type() == Constant::Type::INTEGER) {
    auto ic = dynamic_cast<const Int32Constant *>(&val);
    setInt32(fldName, ic->val());
  } else {
    auto sc = dynamic_cast<const StringConstant *>(&val);
    setString(fldName, sc->val());
  }
}

void TableScan::setInt32(const std::string &fldName, int32_t val) {
  rf_.setInt32(fldName, val);
}

void TableScan::setString(const std::string &fldName, const std::string &val) {
  rf_.setString(fldName, val);
}

void TableScan::insert() { rf_.insert(); }

void TableScan::del() { rf_.del(); }

std::unique_ptr<record::RID> TableScan::getRid() const {
  return rf_.currentRid();
}

void TableScan::moveToRid(const record::RID &rid) { rf_.moveToRid(rid); }

}  // namespace query
}  // namespace minisql