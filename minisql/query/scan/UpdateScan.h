#pragma once

#include <query/scan/Scan.h>
#include <storage/record/RID.h>

namespace minisql {

namespace common {
class Constant;
}

namespace query {

class UpdateScan : public Scan {
 public:
  virtual void insert() = 0;
  virtual void setVal(const std::string& fldName,
                      const common::Constant& val) = 0;
  virtual void setInt32(const std::string& fldName, int32_t val) = 0;
  virtual void setString(const std::string& fldName,
                         const std::string& val) = 0;

  virtual void del() = 0;

  virtual std::unique_ptr<storage::record::RID> getRid() const = 0;
  virtual void moveToRid(const storage::record::RID& rid) = 0;
};

}  // namespace query
}  // namespace minisql