#include <common/Constant.h>
#include <query/Expression.h>
#include <query/Predicate.h>
#include <query/Term.h>
#include <storage/record/Schema.h>
#include <sstream>

namespace minisql {
namespace query {

using namespace storage;

Predicate::Predicate(std::unique_ptr<query::Term> t) {
  terms_.push_back(std::move(t));
}

void Predicate::conjoinWith(const Predicate& pred) {
  for (auto& t : pred.terms_) {
    terms_.push_back(t);
  }
}

bool Predicate::isSatisfied(const Scan& s) const {
  for (auto& t : terms_) {
    if (!t->isSatisfied(s)) {
      return false;
    }
  }
  return true;
}

int32_t Predicate::reductionFactor(const Plan& p) const {
  int32_t factor = 1;
  for (auto& t : terms_) {
    factor *= t->reductionFactor(p);
  }
  return factor;
}

std::shared_ptr<Predicate> Predicate::selectPred(
    const record::Schema& sch) const {
  auto result = std::make_shared<Predicate>();
  for (auto& t : terms_) {
    if (t->appliesTo(sch)) {
      result->terms_.push_back(t);
    }
  }
  if (result->terms_.empty()) {
    return nullptr;
  }
  return result;
}

std::shared_ptr<Predicate> Predicate::joinPred(
    const record::Schema& sch1, const record::Schema& sch2) const {
  record::Schema newSch;
  newSch.addAll(sch1);
  newSch.addAll(sch2);

  auto result = std::make_shared<Predicate>();
  for (auto& t : terms_) {
    if (!t->appliesTo(sch1) && t->appliesTo(sch2) && t->appliesTo(newSch)) {
      result->terms_.push_back(t);
    }
  }

  if (result->terms_.empty()) {
    return nullptr;
  }
  return result;
}

std::shared_ptr<common::Constant> Predicate::equatesWithConstant(
    const std::string& fldName) const {
  for (auto& t : terms_) {
    auto c = t->equatesWithConstant(fldName);
    if (c != nullptr) {
      return c;
    }
  }
  return nullptr;
}

std::string Predicate::equatesWithField(const std::string& fldName) const {
  for (auto& t : terms_) {
    auto s = t->equatesWithField(fldName);
    if (!s.empty()) {
      return s;
    }
  }
  return "";
}

bool Predicate::empty() const { return terms_.empty(); }

std::string Predicate::toString() const {
  if (terms_.empty()) {
    return "";
  }

  std::ostringstream ss;
  ss << terms_[0]->toString();
  for (auto i = 1; i < (int32_t)terms_.size(); i++) {
    ss << " and " << terms_[i]->toString();
  }
  return ss.str();
}

}  // namespace query
}  // namespace minisql