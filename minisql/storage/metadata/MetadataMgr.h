#pragma once

#include <storage/metadata/IndexMgr.h>
#include <storage/metadata/StatMgr.h>
#include <storage/metadata/ViewMgr.h>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}
namespace record {
class Schema;
class TableInfo;
}  // namespace record

namespace metadata {

class StatInfo;
class IndexInfo;
class TableMgr;

class MetadataMgr {
 public:
  MetadataMgr(const MetadataMgr &) = delete;
  MetadataMgr &operator=(const MetadataMgr &) = delete;
  MetadataMgr(MetadataMgr &&) = delete;
  MetadataMgr &operator=(MetadataMgr &&) = delete;

  static MetadataMgr &get();

  void init(bool isNew, tx::Transaction &tx);

  void createTable(const std::string &tblName,
                   const std::shared_ptr<record::Schema> &sch,
                   tx::Transaction &tx);
  std::shared_ptr<record::TableInfo> getTableInfo(const std::string &tblName,
                                                  tx::Transaction &tx);
  void createView(const std::string &viewName, const std::string &viewDef,
                  tx::Transaction &tx);
  std::string getViewDef(const std::string &viewName, tx::Transaction &tx);
  void createIndex(const std::string &idxName, const std::string &tblName,
                   const std::string &fldName, tx::Transaction &tx);
  std::unordered_map<std::string, std::shared_ptr<IndexInfo>> getIndexInfo(
      const std::string &tblName, tx::Transaction &tx);
  std::shared_ptr<StatInfo> getStatInfo(
      const std::string &tblName, const std::shared_ptr<record::TableInfo> &ti,
      tx::Transaction &tx);

 private:
  MetadataMgr() = default;

 private:
  std::shared_ptr<TableMgr> tblMgr_;
  std::unique_ptr<ViewMgr> viewMgr_;
  std::unique_ptr<StatMgr> statMgr_;
  std::unique_ptr<IndexMgr> idxMgr_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql