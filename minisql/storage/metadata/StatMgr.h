#pragma once

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace record {
class TableInfo;
}

namespace metadata {

class TableMgr;
class StatInfo;

class StatMgr {
 public:
  StatMgr(const std::shared_ptr<TableMgr> &tblMgr, tx::Transaction &tx);

  std::shared_ptr<StatInfo> getStatInfo(
      const std::string &tblName, const std::shared_ptr<record::TableInfo> &ti,
      tx::Transaction &tx);

 private:
  void refreshStatistics(tx::Transaction &tx);
  std::shared_ptr<StatInfo> calcTableStats(
      const std::shared_ptr<record::TableInfo> &ti, tx::Transaction &tx);

 private:
  int32_t numCalls_;
  std::mutex mutex_;
  std::shared_ptr<TableMgr> tblMgr_;
  std::unordered_map<std::string, std::shared_ptr<StatInfo>> tblStats_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql