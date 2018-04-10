#include <common/Constant.h>
#include <query/materialize/MergeJoinScan.h>
#include <query/materialize/SortScan.h>

namespace minisql {
namespace query {
namespace materialize {

MergeJoinScan::MergeJoinScan(std::unique_ptr<Scan> s1,
                             std::unique_ptr<SortScan> s2, std::string fldName1,
                             std::string fldName2)
    : s1_(std::move(s1)),
      s2_(std::move(s2)),
      fldName1_(std::move(fldName1)),
      fldName2_(std::move(fldName2)) {
  beforeFirst();
}

void MergeJoinScan::beforeFirst() {
  s1_->beforeFirst();
  s2_->beforeFirst();
}

bool MergeJoinScan::next() {
  bool hasMore2 = s2_->next();
  if (hasMore2 && s2_->getVal(fldName2_)->equals(*joinVal_)) {
    return true;
  }

  bool hasMore1 = s1_->next();
  if (hasMore1 && s1_->getVal(fldName1_)->equals(*joinVal_)) {
    s2_->restorePosition();
    return true;
  }

  while (hasMore1 && hasMore2) {
    auto v1 = s1_->getVal(fldName1_);
    auto v2 = s2_->getVal(fldName2_);
    if (v1->compareTo(*v2) < 0) {
      hasMore1 = s1_->next();
    } else if (v1->compareTo(*v2) > 0) {
      hasMore2 = s2_->next();
    } else {
      s2_->savePosition();
      joinVal_ = s2_->getVal(fldName2_);
      return true;
    }
  }

  return false;
}

void MergeJoinScan::close() {
  s1_->close();
  s2_->close();
}

std::shared_ptr<common::Constant> MergeJoinScan::getVal(
    const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getVal(fldName);
  } else {
    return s2_->getVal(fldName);
  }
}

int32_t MergeJoinScan::getInt32(const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getInt32(fldName);
  } else {
    return s2_->getInt32(fldName);
  }
}

std::string MergeJoinScan::getString(const std::string &fldName) const {
  if (s1_->hasField(fldName)) {
    return s1_->getString(fldName);
  } else {
    return s2_->getString(fldName);
  }
}

bool MergeJoinScan::hasField(const std::string &fldName) const {
  return s1_->hasField(fldName) || s2_->hasField(fldName);
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql