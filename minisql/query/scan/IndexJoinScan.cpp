#include <query/scan/IndexJoinScan.h>
#include <query/scan/TableScan.h>
#include <storage/index/Index.h>

namespace minisql {
namespace query {

IndexJoinScan::IndexJoinScan(std::unique_ptr<Scan> s,
                             std::unique_ptr<storage::index::Index> idx,
                             std::string joinField,
                             std::unique_ptr<TableScan> ts)
    : joinField_(std::move(joinField)),
      s_(std::move(s)),
      ts_(std::move(ts)),
      idx_(std::move(idx)) {
  beforeFirst();
}

void IndexJoinScan::beforeFirst() {
  s_->beforeFirst();
  s_->next();
}

bool IndexJoinScan::next() {
  while (true) {
    if (idx_->next()) {
      ts_->moveToRid(*idx_->getDataRid());
      return true;
    }
    if (!s_->next()) {
      return false;
    }
    resetIndex();
  }
}

void IndexJoinScan::close() {
  s_->close();
  idx_->close();
  ts_->close();
}

std::shared_ptr<common::Constant> IndexJoinScan::getVal(
    const std::string &fldName) const {
  if (ts_->hasField(fldName)) {
    return ts_->getVal(fldName);
  } else {
    return s_->getVal(fldName);
  }
}

int32_t IndexJoinScan::getInt32(const std::string &fldName) const {
  if (ts_->hasField(fldName)) {
    return ts_->getInt32(fldName);
  } else {
    return s_->getInt32(fldName);
  }
}

std::string IndexJoinScan::getString(const std::string &fldName) const {
  if (ts_->hasField(fldName)) {
    return ts_->getString(fldName);
  } else {
    return s_->getString(fldName);
  }
}

bool IndexJoinScan::hasField(const std::string &fldName) const {
  return ts_->hasField(fldName) || s_->hasField(fldName);
}

void IndexJoinScan::resetIndex() {
  auto searchKey = s_->getVal(joinField_);
  idx_->beforeFirst(searchKey);
}

}  // namespace query
}  // namespace minisql