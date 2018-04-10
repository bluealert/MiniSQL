#pragma once

#include <query/materialize/GroupValue.h>
#include <query/scan/Scan.h>
#include <vector>

namespace minisql {
namespace query {
namespace materialize {

class AggregationFn;

class GroupByScan : public Scan {
 public:
  GroupByScan(std::unique_ptr<Scan> s,
              const std::vector<std::string> &groupFields,
              const std::vector<std::shared_ptr<AggregationFn>> &aggFns);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string &fldName) const override;
  int32_t getInt32(const std::string &fldName) const override;
  std::string getString(const std::string &fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string &fldName) const override;

  void close() override;

 private:
  bool moreGroups_;
  std::unique_ptr<Scan> s_;
  std::unique_ptr<GroupValue> groupVal_;
  std::vector<std::string> groupFields_;
  std::vector<std::shared_ptr<AggregationFn>> aggFns_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql