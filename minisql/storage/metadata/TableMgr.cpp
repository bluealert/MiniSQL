#include <glog/logging.h>
#include <storage/metadata/TableMgr.h>
#include <storage/record/RecordFile.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace storage {
namespace metadata {

TableMgr::TableMgr(bool isNew, tx::Transaction &tx) {
  auto tblCatSch = std::make_shared<record::Schema>();
  tblCatSch->addStringField("tblname", MAX_NAME);
  tblCatSch->addInt32Field("reclength");
  tblCatInfo_ = std::make_unique<record::TableInfo>("tblcat", tblCatSch);

  auto fldCatSch = std::make_shared<record::Schema>();
  fldCatSch->addStringField("tblname", MAX_NAME);
  fldCatSch->addStringField("fldname", MAX_NAME);
  fldCatSch->addInt32Field("type");
  fldCatSch->addInt32Field("length");
  fldCatSch->addInt32Field("offset");
  fldCatInfo_ = std::make_unique<record::TableInfo>("fldcat", fldCatSch);

  if (isNew) {
    createTable("tblcat", tblCatSch, tx);
    createTable("fldcat", fldCatSch, tx);
  }
}

void TableMgr::createTable(const std::string &tblName,
                           const std::shared_ptr<record::Schema> &sch,
                           tx::Transaction &tx) {
  int32_t recLen = getRecordLengthByTblName(tblName, tx);
  if (recLen != -1) {
    LOG(ERROR) << "TableMgr. table already exist. name: " << tblName;
    return;
  }

  record::TableInfo ti(tblName, sch);

  record::RecordFile tblCatFile(tblCatInfo_, tx);
  tblCatFile.insert();
  tblCatFile.setString("tblname", tblName);
  tblCatFile.setInt32("reclength", ti.recordLength());
  tblCatFile.close();

  record::RecordFile fldCatFile(fldCatInfo_, tx);
  for (const auto &fldName : sch->fields()) {
    fldCatFile.insert();
    fldCatFile.setString("tblname", tblName);
    fldCatFile.setString("fldname", fldName);
    fldCatFile.setInt32("type", (int32_t)sch->type(fldName));
    fldCatFile.setInt32("length", sch->length(fldName));
    fldCatFile.setInt32("offset", ti.offset(fldName));
  }
  fldCatFile.close();
}

std::shared_ptr<record::TableInfo> TableMgr::getTableInfo(
    const std::string &tblName, tx::Transaction &tx) {
  int32_t recLen = getRecordLengthByTblName(tblName, tx);
  if (recLen == -1) {
    throw std::runtime_error("TableMgr. can't find table. name: " + tblName);
  }

  record::RecordFile fldCatFile(fldCatInfo_, tx);
  auto sch = std::make_shared<record::Schema>();
  std::unordered_map<std::string, int32_t> offsets;
  while (fldCatFile.next()) {
    if (fldCatFile.getString("tblname") == tblName) {
      auto fldName = fldCatFile.getString("fldname");
      auto fldType = fldCatFile.getInt32("type");
      auto fldLen = fldCatFile.getInt32("length");
      auto offset = fldCatFile.getInt32("offset");
      offsets[fldName] = offset;
      sch->addField(fldName, (record::Schema::Type)fldType, fldLen);
    }
  }
  fldCatFile.close();
  return std::make_shared<record::TableInfo>(tblName, sch, offsets, recLen);
}

int32_t TableMgr::getRecordLengthByTblName(const std::string &tblName,
                                           tx::Transaction &tx) {
  int32_t recLen = -1;
  record::RecordFile tcatFile(tblCatInfo_, tx);
  while (tcatFile.next()) {
    if (tcatFile.getString("tblname") == tblName) {
      recLen = tcatFile.getInt32("reclength");
      break;
    }
  }
  tcatFile.close();

  return recLen;
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql