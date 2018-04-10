#pragma once

#include <query/scan/Scan.h>

namespace minisql {
namespace query {

class ProductScan : public Scan {
 public:
  ProductScan(std::unique_ptr<Scan> s1, std::unique_ptr<Scan> s2);

  void beforeFirst() override;
  bool next() override;

  bool hasField(const std::string& fldName) const override;
  int32_t getInt32(const std::string& fldName) const override;
  std::string getString(const std::string& fldName) const override;

  std::shared_ptr<common::Constant> getVal(
      const std::string& fldName) const override;

  void close() override;

 private:
  std::unique_ptr<Scan> s1_, s2_;
};

}  // namespace query
}  // namespace minisql