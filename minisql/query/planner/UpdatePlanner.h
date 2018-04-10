#pragma once

#include <query/parse/ParseData.h>

namespace minisql {

namespace storage {
namespace tx {
class Transaction;
}
}  // namespace storage

namespace query {
namespace planner {

class UpdatePlanner {
 public:
  virtual ~UpdatePlanner() = default;

  virtual int32_t executeInsert(parse::InsertData &data,
                                storage::tx::Transaction &tx) = 0;
  virtual int32_t executeDelete(parse::DeleteData &data,
                                storage::tx::Transaction &tx) = 0;
  virtual int32_t executeModify(parse::ModifyData &data,
                                storage::tx::Transaction &tx) = 0;
  virtual int32_t executeCreateTable(parse::CreateTableData &data,
                                     storage::tx::Transaction &tx) = 0;
  virtual int32_t executeCreateView(parse::CreateViewData &data,
                                    storage::tx::Transaction &tx) = 0;
  virtual int32_t executeCreateIndex(parse::CreateIndexData &data,
                                     storage::tx::Transaction &tx) = 0;
};

}  // namespace planner
}  // namespace query
}  // namespace minisql