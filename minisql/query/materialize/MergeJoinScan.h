#pragma once

#include <query/scan/Scan.h>

namespace minisql {
namespace query {
namespace materialize {

class SortScan;

class MergeJoinScan : public Scan {
 public:
  MergeJoinScan(std::unique_ptr<Scan> s1, std::unique_ptr<SortScan> s2,
                std::string fldName1, std::string fldName2);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string &fldName) const override;
  int32_t getInt32(const std::string &fldName) const override;
  std::string getString(const std::string &fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string &fldName) const override;

  void close() override;

 private:
  std::unique_ptr<Scan> s1_;
  std::unique_ptr<SortScan> s2_;
  std::string fldName1_, fldName2_;
  std::shared_ptr<common::Constant> joinVal_;
};

}  // namespace materialize
}  // namespace query
}  // namespace minisql