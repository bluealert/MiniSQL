#include <storage/metadata/StatInfo.h>
#include <storage/metadata/StatMgr.h>
#include <storage/metadata/TableMgr.h>
#include <storage/record/RecordFile.h>

namespace minisql {
namespace storage {
namespace metadata {

StatMgr::StatMgr(const std::shared_ptr<TableMgr> &tblMgr, tx::Transaction &tx)
    : numCalls_(0), tblMgr_(tblMgr) {
  refreshStatistics(tx);
}

std::shared_ptr<StatInfo> StatMgr::getStatInfo(
    const std::string &tblName, const std::shared_ptr<record::TableInfo> &ti,
    tx::Transaction &tx) {
  std::lock_guard<std::mutex> lk(mutex_);

  numCalls_++;
  if (numCalls_ > 100) {
    refreshStatistics(tx);
  }

  auto it = tblStats_.find(tblName);
  if (it == tblStats_.end()) {
    auto si = calcTableStats(ti, tx);
    tblStats_[tblName] = si;
    return si;
  } else {
    return it->second;
  }
}

void StatMgr::refreshStatistics(tx::Transaction &tx) {
  tblStats_.clear();
  numCalls_ = 0;
  auto ti = tblMgr_->getTableInfo("tblcat", tx);
  record::RecordFile rf(ti, tx);
  while (rf.next()) {
    std::string tblName = rf.getString("tblname");
    auto ti2 = tblMgr_->getTableInfo(tblName, tx);
    auto si = calcTableStats(ti2, tx);
    tblStats_[tblName] = si;
  }
  rf.close();
}

std::shared_ptr<StatInfo> StatMgr::calcTableStats(
    const std::shared_ptr<record::TableInfo> &ti, tx::Transaction &tx) {
  int32_t numRecs = 0, numBlocks = 0;
  record::RecordFile rf(ti, tx);
  while (rf.next()) {
    numRecs++;
    numBlocks = rf.currentRid()->blockNumber() + 1;
  }
  rf.close();
  return std::make_shared<StatInfo>(numBlocks, numRecs);
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql