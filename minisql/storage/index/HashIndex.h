#pragma once

#include <query/scan/TableScan.h>
#include <storage/index/Index.h>
#include <storage/record/RID.h>
#include <storage/record/Schema.h>
#include <string>

namespace minisql {

namespace common {
class Constant;
}

namespace storage {

namespace tx {
class Transaction;
}

namespace index {

class HashIndex : public Index {
 public:
  HashIndex(std::string idxName, const std::shared_ptr<record::Schema> &sch,
            tx::Transaction &tx);

  void beforeFirst(const std::shared_ptr<common::Constant> &searchKey) override;
  bool next() override;
  std::unique_ptr<record::RID> getDataRid() override;
  void insert(const std::shared_ptr<common::Constant> &dataVal,
              const record::RID &dataRid) override;
  void del(const std::shared_ptr<common::Constant> &dataVal,
           const record::RID &dataRid) override;
  void close() override;

  static int32_t searchCost(int32_t numBlocks, int32_t rpb);

 private:
  std::string idxName_;
  std::shared_ptr<record::Schema> sch_;
  tx::Transaction &tx_;
  std::shared_ptr<common::Constant> searchKey_;
  std::unique_ptr<query::TableScan> ts_;
};

}  // namespace index
}  // namespace storage
}  // namespace minisql