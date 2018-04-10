#include <query/plan/SelectPlan.h>
#include <query/plan/TablePlan.h>
#include <query/planner/IndexUpdatePlanner.h>
#include <query/scan/UpdateScan.h>
#include <storage/index/Index.h>
#include <storage/metadata/IndexInfo.h>
#include <storage/metadata/MetadataMgr.h>

namespace minisql {
namespace query {
namespace planner {

int32_t IndexUpdatePlanner::executeInsert(parse::InsertData &data,
                                          storage::tx::Transaction &tx) {
  auto tblName = data.tblName();
  auto p = std::make_shared<query::TablePlan>(tblName, tx);
  auto us = dynamic_cast<query::UpdateScan *>(p->open().get());

  // first, insert the record
  us->insert();
  auto rid = us->getRid();

  // then modify each field, inserting an index record if appropriate
  auto indexes =
      storage::metadata::MetadataMgr::get().getIndexInfo(tblName, tx);
  auto it = data.vals().begin();
  for (auto &fldName : data.flds()) {
    auto val = *it++;
    us->setVal(fldName, *val);
    auto it2 = indexes.find(fldName);
    if (it2 != indexes.end()) {
      auto idx = it2->second->open();
      idx->insert(val, *rid);
    }
  }
  us->close();
  return 1;
}

int32_t IndexUpdatePlanner::executeDelete(parse::DeleteData &data,
                                          storage::tx::Transaction &tx) {
  auto tblName = data.tblName();
  auto p1 = std::make_shared<query::TablePlan>(tblName, tx);
  auto p2 = std::make_shared<query::SelectPlan>(p1, data.pred());
  auto indexes =
      storage::metadata::MetadataMgr::get().getIndexInfo(tblName, tx);

  auto us = dynamic_cast<query::UpdateScan *>(p2->open().get());
  int32_t count = 0;
  while (us->next()) {
    auto rid = us->getRid();
    for (auto &kv : indexes) {
      auto val = us->getVal(kv.first);
      auto idx = indexes[kv.first]->open();
      idx->del(val, *rid);
      idx->close();
    }
    us->del();
    count++;
  }
  us->close();
  return count;
}

int32_t IndexUpdatePlanner::executeModify(parse::ModifyData &data,
                                          storage::tx::Transaction &tx) {
  auto p1 = std::make_shared<query::TablePlan>(data.tblName(), tx);
  auto p2 = std::make_shared<query::SelectPlan>(p1, data.pred());

  auto idxInfo =
      storage::metadata::MetadataMgr::get().getIndexInfo(data.tblName(), tx);
  auto it = idxInfo.find(data.fldName());
  auto idx = (it != idxInfo.end()) ? it->second->open() : nullptr;

  auto us = dynamic_cast<query::UpdateScan *>(p2->open().get());
  int32_t count = 0;
  while (us->next()) {
    auto newVal = data.newVal().evaluate(*us);
    auto oldVal = us->getVal(data.fldName());
    us->setVal(data.fldName(), *newVal);

    if (idx != nullptr) {
      auto rid = us->getRid();
      idx->del(oldVal, *rid);
      idx->insert(newVal, *rid);
    }
    count++;
  }
  if (idx != nullptr) {
    idx->close();
  }
  us->close();
  return count;
}

int32_t IndexUpdatePlanner::executeCreateTable(parse::CreateTableData &data,
                                               storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createTable(data.tblName(), data.sch(),
                                                    tx);
  return 0;
}

int32_t IndexUpdatePlanner::executeCreateView(parse::CreateViewData &data,
                                              storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createView(data.viewName(),
                                                   data.viewDef(), tx);
  return 0;
}

int32_t IndexUpdatePlanner::executeCreateIndex(parse::CreateIndexData &data,
                                               storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createIndex(
      data.idxName(), data.tblName(), data.fldName(), tx);
  return 0;
}

}  // namespace planner
}  // namespace query
}  // namespace minisql