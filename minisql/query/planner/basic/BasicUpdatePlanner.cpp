#include <query/plan/SelectPlan.h>
#include <query/plan/TablePlan.h>
#include <query/planner/basic/BasicUpdatePlanner.h>
#include <query/scan/TableScan.h>
#include <storage/metadata/MetadataMgr.h>

namespace minisql {
namespace query {
namespace planner {
namespace basic {

int32_t BasicUpdatePlanner::executeInsert(parse::InsertData &data,
                                          storage::tx::Transaction &tx) {
  auto p = std::make_shared<query::TablePlan>(data.tblName(), tx);
  auto s = p->open();
  auto ts = dynamic_cast<query::TableScan *>(s.get());
  ts->insert();
  for (size_t i = 0; i < data.flds().size(); i++) {
    ts->setVal(data.flds()[i], *(data.vals()[i]));
  }
  return 1;
}

int32_t BasicUpdatePlanner::executeDelete(parse::DeleteData &data,
                                          storage::tx::Transaction &tx) {
  auto p1 = std::make_shared<query::TablePlan>(data.tblName(), tx);
  auto p2 = std::make_shared<query::SelectPlan>(p1, data.pred());
  auto s = p2->open();
  auto us = dynamic_cast<query::UpdateScan *>(s.get());
  int32_t count = 0;
  while (us->next()) {
    us->del();
    count++;
  }
  us->close();
  return count;
}

int32_t BasicUpdatePlanner::executeModify(parse::ModifyData &data,
                                          storage::tx::Transaction &tx) {
  auto p1 = std::make_shared<query::TablePlan>(data.tblName(), tx);
  auto p2 = std::make_shared<query::SelectPlan>(p1, data.pred());
  auto s = p2->open();
  auto us = dynamic_cast<query::UpdateScan *>(s.get());
  int32_t count = 0;
  while (us->next()) {
    auto val = data.newVal().evaluate(*us);
    us->setVal(data.fldName(), *val);
    count++;
  }
  us->close();
  return count;
}

int32_t BasicUpdatePlanner::executeCreateTable(parse::CreateTableData &data,
                                               storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createTable(data.tblName(), data.sch(),
                                                    tx);
  return 0;
}

int32_t BasicUpdatePlanner::executeCreateView(parse::CreateViewData &data,
                                              storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createView(data.viewName(),
                                                   data.viewDef(), tx);
  return 0;
}

int32_t BasicUpdatePlanner::executeCreateIndex(parse::CreateIndexData &data,
                                               storage::tx::Transaction &tx) {
  storage::metadata::MetadataMgr::get().createIndex(
      data.idxName(), data.tblName(), data.fldName(), tx);
  return 0;
}

}  // namespace basic
}  // namespace planner
}  // namespace query
}  // namespace minisql