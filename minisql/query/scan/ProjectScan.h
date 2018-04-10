#pragma once

#include <query/scan/Scan.h>
#include <vector>

namespace minisql {
namespace query {

class ProjectScan : public Scan {
 public:
  explicit ProjectScan(std::unique_ptr<Scan> s,
                       const std::vector<std::string>& fieldList);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string& fldName) const override;
  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  void close() override;

 private:
  std::unique_ptr<Scan> s_;
  std::vector<std::string> fieldList_;
};

}  // namespace query
}  // namespace minisql