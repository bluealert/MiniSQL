#include <storage/metadata/IndexInfo.h>
#include <storage/metadata/IndexMgr.h>
#include <storage/metadata/TableMgr.h>
#include <storage/record/RecordFile.h>

namespace minisql {
namespace storage {
namespace metadata {

IndexMgr::IndexMgr(bool isNew, const std::shared_ptr<TableMgr> &tblMgr,
                   tx::Transaction &tx) {
  if (isNew) {
    auto sch = std::make_shared<record::Schema>();
    sch->addStringField("indexname", TableMgr::MAX_NAME);
    sch->addStringField("tablename", TableMgr::MAX_NAME);
    sch->addStringField("fieldname", TableMgr::MAX_NAME);
    tblMgr->createTable("idxcat", sch, tx);
  }
  ti_ = tblMgr->getTableInfo("idxcat", tx);
}

void IndexMgr::createIndex(const std::string &idxName,
                           const std::string &tblName,
                           const std::string &fldName, tx::Transaction &tx) {
  record::RecordFile rf(ti_, tx);
  rf.insert();
  rf.setString("indexname", idxName);
  rf.setString("tablename", tblName);
  rf.setString("fieldname", fldName);
  rf.close();
}

std::unordered_map<std::string, std::shared_ptr<IndexInfo>>
IndexMgr::getIndexInfo(const std::string &tblName, tx::Transaction &tx) {
  std::unordered_map<std::string, std::shared_ptr<IndexInfo>> result;
  record::RecordFile rf(ti_, tx);
  while (rf.next()) {
    if (rf.getString("tablename") == tblName) {
      auto idxName = rf.getString("indexname");
      auto fldName = rf.getString("fieldname");
      auto ii = std::make_shared<IndexInfo>(idxName, tblName, fldName, tx);
      result.emplace(std::make_pair(fldName, ii));
    }
  }
  rf.close();
  return result;
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql