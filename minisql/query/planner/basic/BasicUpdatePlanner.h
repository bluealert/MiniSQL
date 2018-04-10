#pragma once

#include <query/planner/UpdatePlanner.h>

namespace minisql {
namespace query {
namespace planner {
namespace basic {

class BasicUpdatePlanner : public UpdatePlanner {
  int32_t executeInsert(parse::InsertData &data,
                        storage::tx::Transaction &tx) override;
  int32_t executeDelete(parse::DeleteData &data,
                        storage::tx::Transaction &tx) override;
  int32_t executeModify(parse::ModifyData &data,
                        storage::tx::Transaction &tx) override;
  int32_t executeCreateTable(parse::CreateTableData &data,
                             storage::tx::Transaction &tx) override;
  int32_t executeCreateView(parse::CreateViewData &data,
                            storage::tx::Transaction &tx) override;
  int32_t executeCreateIndex(parse::CreateIndexData &data,
                             storage::tx::Transaction &tx) override;
};

}  // namespace basic
}  // namespace planner
}  // namespace query
}  // namespace minisql