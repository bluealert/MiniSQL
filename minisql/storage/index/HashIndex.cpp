#include <common/Constant.h>
#include <storage/index/HashIndex.h>
#include <storage/record/TableInfo.h>
#include <sstream>

namespace minisql {
namespace storage {
namespace index {

static int32_t NUM_BLOCKS{100};

HashIndex::HashIndex(std::string idxName,
                     const std::shared_ptr<record::Schema>& sch,
                     tx::Transaction& tx)
    : idxName_(std::move(idxName)), sch_(sch), tx_(tx) {}

void HashIndex::beforeFirst(
    const std::shared_ptr<common::Constant>& searchKey) {
  close();
  searchKey_ = searchKey;
  auto bucket = searchKey->hashCode() % NUM_BLOCKS;
  std::ostringstream os;
  os << idxName_ << bucket;
  auto ti = std::make_shared<record::TableInfo>(os.str(), sch_);
  ts_ = std::make_unique<query::TableScan>(ti, tx_);
}

bool HashIndex::next() {
  while (ts_->next()) {
    if (ts_->getVal("dataval")->equals(*searchKey_)) {
      return true;
    }
  }
  return false;
}

std::unique_ptr<record::RID> HashIndex::getDataRid() {
  auto blkNum = ts_->getInt32("block");
  auto id = ts_->getInt32("id");
  return std::make_unique<record::RID>(blkNum, id);
}

void HashIndex::insert(const std::shared_ptr<common::Constant>& dataVal,
                       const record::RID& dataRid) {
  beforeFirst(dataVal);
  ts_->insert();
  ts_->setInt32("block", dataRid.blockNumber());
  ts_->setInt32("id", dataRid.slotNumber());
  ts_->setVal("dataval", *dataVal);
}

void HashIndex::del(const std::shared_ptr<common::Constant>& dataVal,
                    const record::RID& dataRid) {
  beforeFirst(dataVal);
  while (next()) {
    if (getDataRid()->equals(dataRid)) {
      ts_->del();
      return;
    }
  }
}

void HashIndex::close() {
  if (ts_ != nullptr) {
    ts_->close();
  }
}

int32_t HashIndex::searchCost(int32_t numBlocks, int32_t rpb) {
  return numBlocks / NUM_BLOCKS;
}

}  // namespace index
}  // namespace storage
}  // namespace minisql