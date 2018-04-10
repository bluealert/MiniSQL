#include <storage/metadata/IndexInfo.h>
#include <storage/metadata/MetadataMgr.h>
#include <storage/metadata/TableMgr.h>

namespace minisql {
namespace storage {
namespace metadata {

MetadataMgr &MetadataMgr::get() {
  static MetadataMgr inst;
  return inst;
}

void MetadataMgr::init(bool isNew, tx::Transaction &tx) {
  tblMgr_ = std::make_shared<TableMgr>(isNew, tx);
  viewMgr_ = std::make_unique<ViewMgr>(isNew, tblMgr_, tx);
  statMgr_ = std::make_unique<StatMgr>(tblMgr_, tx);
  idxMgr_ = std::make_unique<IndexMgr>(isNew, tblMgr_, tx);
}

void MetadataMgr::createTable(const std::string &tblName,
                              const std::shared_ptr<record::Schema> &sch,
                              tx::Transaction &tx) {
  tblMgr_->createTable(tblName, sch, tx);
}

std::shared_ptr<record::TableInfo> MetadataMgr::getTableInfo(
    const std::string &tblName, tx::Transaction &tx) {
  return tblMgr_->getTableInfo(tblName, tx);
}

void MetadataMgr::createView(const std::string &viewName,
                             const std::string &viewDef, tx::Transaction &tx) {
  viewMgr_->createView(viewName, viewDef, tx);
}

std::string MetadataMgr::getViewDef(const std::string &viewName,
                                    tx::Transaction &tx) {
  return viewMgr_->getViewDef(viewName, tx);
}

void MetadataMgr::createIndex(const std::string &idxName,
                              const std::string &tblName,
                              const std::string &fldName, tx::Transaction &tx) {
  idxMgr_->createIndex(idxName, tblName, fldName, tx);
}

std::unordered_map<std::string, std::shared_ptr<IndexInfo>>
MetadataMgr::getIndexInfo(const std::string &tblName, tx::Transaction &tx) {
  return idxMgr_->getIndexInfo(tblName, tx);
}

std::shared_ptr<StatInfo> MetadataMgr::getStatInfo(
    const std::string &tblName, const std::shared_ptr<record::TableInfo> &ti,
    tx::Transaction &tx) {
  return statMgr_->getStatInfo(tblName, ti, tx);
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql