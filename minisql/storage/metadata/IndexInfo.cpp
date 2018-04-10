#include <storage/file/Page.h>
#include <storage/index/HashIndex.h>
#include <storage/metadata/IndexInfo.h>

namespace minisql {
namespace storage {
namespace metadata {

IndexInfo::IndexInfo(const std::string &idxName, const std::string &tblName,
                     const std::string &fldName, tx::Transaction &tx)
    : idxName_(idxName), fldName_(fldName), tx_(tx) {}

std::unique_ptr<index::Index> IndexInfo::open() {
  auto sch = schema();
  return std::make_unique<index::HashIndex>(idxName_, std::move(sch), tx_);
}

int32_t IndexInfo::blocksAccessed() const {
  auto idxTi = std::make_unique<record::TableInfo>("", schema());
  int32_t rpb = file::Page::BLOCK_SIZE / idxTi->recordLength();
  int32_t numBlocks = si_->recordsOutput() / rpb;
  return index::HashIndex::searchCost(numBlocks, rpb);
}

int32_t IndexInfo::recordsOutput() const {
  return si_->recordsOutput() / si_->distinctValues(fldName_);
}

int32_t IndexInfo::distinctValues(const std::string &fldName) const {
  if (fldName_ == fldName) {
    return 1;
  } else {
    return std::min(si_->distinctValues(fldName), recordsOutput());
  }
}

std::shared_ptr<record::Schema> IndexInfo::schema() const {
  auto sch = std::make_shared<record::Schema>();
  sch->addInt32Field("block");
  sch->addInt32Field("id");
  if (ti_->schema()->type(fldName_) == record::Schema::Type::INT32) {
    sch->addInt32Field("dataval");
  } else {
    auto fldLen = ti_->schema()->length(fldName_);
    sch->addStringField("dataval", fldLen);
  }
  return sch;
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql