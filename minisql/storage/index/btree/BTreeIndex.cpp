#include <limits>
#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <storage/file/Block.h>
#include <storage/index/btree/BTPageFormatter.h>
#include <storage/index/btree/BTreeDir.h>
#include <storage/index/btree/BTreeIndex.h>
#include <storage/index/btree/BTreeLeaf.h>
#include <storage/index/btree/BTreePage.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

namespace minisql {
namespace storage {
namespace index {
namespace btree {

BTreeIndex::BTreeIndex(const std::string& idxName,
                       const std::shared_ptr<record::Schema>& leafSch,
                       tx::Transaction& tx)
    : tx_(tx) {
  auto leafTbl = idxName + "leaf";
  leafTi_ = std::make_shared<record::TableInfo>(leafTbl, leafSch);
  if (tx_.blockCount(leafTi_->fileName()) == 0) {
    BTPageFormatter fmtr(*leafTi_, -1);
    tx_.pinNew(leafTi_->fileName(), fmtr);
  }

  auto dirSch = std::make_shared<record::Schema>();
  dirSch->add("block", *leafSch);
  dirSch->add("dataval", *leafSch);

  auto dirTbl = idxName + "dir";
  dirTi_ = std::make_shared<record::TableInfo>(dirTbl, dirSch);
  rootBlk_ = std::make_shared<file::Block>(dirTi_->fileName(), 0);
  if (tx_.blockCount(dirTi_->fileName()) == 0) {
    BTPageFormatter fmtr(*dirTi_, 0);
    tx_.pinNew(dirTi_->fileName(), fmtr);
  }

  BTreePage page(rootBlk_, dirTi_, tx);
  if (page.getNumRecs() == 0) {
    auto fldType = dirSch->type("dataval");
    auto maxInt32 = std::numeric_limits<int32_t>::max();
    std::shared_ptr<common::Constant> minVal;
    if (fldType == record::Schema::Type::INT32) {
      minVal = std::make_shared<common::Int32Constant>(maxInt32);
    } else {
      minVal = std::make_shared<common::StringConstant>("");
    }
    page.insertDir(0, minVal, 0);
  }
  page.close();
}

void BTreeIndex::beforeFirst(
    const std::shared_ptr<common::Constant>& searchKey) {
  close();
  BTreeDir root(rootBlk_, dirTi_, tx_);
  auto blkNum = root.search(searchKey);
  root.close();
  auto leafBlk = std::make_shared<file::Block>(leafTi_->fileName(), blkNum);
  leaf_ = std::make_shared<BTreeLeaf>(leafBlk, leafTi_, searchKey, tx_);
}

bool BTreeIndex::next() { return leaf_->next(); }

std::unique_ptr<record::RID> BTreeIndex::getDataRid() {
  return leaf_->getDataRid();
}

void BTreeIndex::insert(const std::shared_ptr<common::Constant>& dataVal,
                        const std::shared_ptr<record::RID>& dataRid) {
  beforeFirst(dataVal);
  auto e = leaf_->insert(dataRid);
  leaf_->close();
  if (e == nullptr) {
    return;
  }

  BTreeDir root(rootBlk_, dirTi_, tx_);
  auto e2 = root.insert(e);
  if (e2 != nullptr) {
    root.makeNewRoot(e2);
  }
  root.close();
}

void BTreeIndex::del(const std::shared_ptr<common::Constant>& dataVal,
                     const std::shared_ptr<record::RID>& dataRid) {
  beforeFirst(dataVal);
  leaf_->del(dataRid);
  leaf_->close();
}

void BTreeIndex::close() {
  if (leaf_ != nullptr) {
    leaf_->close();
  }
}

}  // namespace btree
}  // namespace index
}  // namespace storage
}  // namespace minisql