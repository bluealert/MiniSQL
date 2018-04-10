#pragma once

#include <storage/record/RID.h>
#include <memory>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {
namespace index {

class Index {
 public:
  virtual ~Index() = default;

  virtual void beforeFirst(
      const std::shared_ptr<common::Constant>& searchKey) = 0;
  virtual bool next() = 0;
  virtual std::unique_ptr<record::RID> getDataRid() = 0;
  virtual void insert(const std::shared_ptr<common::Constant>& dataVal,
                      const record::RID& dataRid) = 0;
  virtual void del(const std::shared_ptr<common::Constant>& dataVal,
                   const record::RID& dataRid) = 0;
  virtual void close() = 0;
};

}  // namespace index
}  // namespace storage
}  // namespace minisql