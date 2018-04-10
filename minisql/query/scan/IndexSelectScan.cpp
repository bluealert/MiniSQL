#include <query/scan/IndexSelectScan.h>
#include <query/scan/TableScan.h>
#include <storage/index/Index.h>

namespace minisql {
namespace query {

IndexSelectScan::IndexSelectScan(std::unique_ptr<storage::index::Index> idx,
                                 const std::shared_ptr<common::Constant> &val,
                                 std::unique_ptr<TableScan> ts)
    : idx_(std::move(idx)), val_(val), ts_(std::move(ts)) {}

void IndexSelectScan::beforeFirst() { idx_->beforeFirst(val_); }

bool IndexSelectScan::next() {
  bool ok = idx_->next();
  if (ok) {
    ts_->moveToRid(*idx_->getDataRid());
  }
  return ok;
}

void IndexSelectScan::close() {
  idx_->close();
  ts_->close();
}

std::shared_ptr<common::Constant> IndexSelectScan::getVal(
    const std::string &fldName) const {
  return ts_->getVal(fldName);
}

int32_t IndexSelectScan::getInt32(const std::string &fldName) const {
  return ts_->getInt32(fldName);
}

std::string IndexSelectScan::getString(const std::string &fldName) const {
  return ts_->getString(fldName);
}

bool IndexSelectScan::hasField(const std::string &fldName) const {
  return ts_->hasField(fldName);
}

}  // namespace query
}  // namespace minisql