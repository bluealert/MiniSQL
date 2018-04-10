#include <query/materialize/RecordComparator.h>
#include <query/materialize/SortScan.h>
#include <query/materialize/TempTable.h>

namespace minisql {
namespace query {
namespace materialize {

SortScan::SortScan(const std::deque<std::shared_ptr<TempTable>> &runs,
                   RecordComparator &comp)
    : s1_(nullptr),
      s2_(nullptr),
      currScan_(nullptr),
      comp_(comp),
      hasMore1_(false),
      hasMore2_(false) {
  s1_ = runs[0]->open();
  hasMore1_ = s1_->next();
  if (runs.size() > 1) {
    s2_ = runs[1]->open();
    hasMore2_ = s2_->next();
  }
}

void SortScan::beforeFirst() {
  currScan_ = nullptr;
  s1_->beforeFirst();
  hasMore1_ = s1_->next();
  if (s2_ != nullptr) {
    s2_->beforeFirst();
    hasMore2_ = s2_->next();
  }
}

bool SortScan::next() {
  if (currScan_ != nullptr) {
    if (currScan_ == s1_) {
      hasMore1_ = s1_->next();
    } else if (currScan_ == s2_) {
      hasMore2_ = s2_->next();
    }
  }

  if (!hasMore1_ && !hasMore2_) {
    return false;
  }

  if (hasMore1_ && hasMore2_) {
    if (comp_.compare(*s1_, *s2_) < 0) {
      currScan_ = s1_;
    } else {
      currScan_ = s2_;
    }
  } else if (hasMore1_) {
    currScan_ = s1_;
  } else {
    currScan_ = s2_;
  }

  return true;
}

void SortScan::close() {
  s1_->close();
  if (s2_ != nullptr) {
    s2_->close();
  }
}

std::shared_ptr<common::Constant> SortScan::getVal(
    const std::string &fldName) const {
  return currScan_->getVal(fldName);
}

int32_t SortScan::getInt32(const std::string &fldName) const {
  return currScan_->getInt32(fldName);
}

std::string SortScan::getString(const std::string &fldName) const {
  return currScan_->getString(fldName);
}

bool SortScan::hasField(const std::string &fldName) const {
  return currScan_->hasField(fldName);
}

void SortScan::savePosition() {
  auto rid1 = s1_->getRid();
  auto rid2 = (s2_ == nullptr) ? nullptr : s2_->getRid();
  savedPos_.clear();
  savedPos_.push_back(std::move(rid1));
  savedPos_.push_back(std::move(rid2));
}

void SortScan::restorePosition() {
  auto rid1 = std::move(savedPos_[0]);
  auto rid2 = std::move(savedPos_[1]);
  s1_->moveToRid(*rid1);
  if (rid2 != nullptr) {
    s2_->moveToRid(*rid2);
  }
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql