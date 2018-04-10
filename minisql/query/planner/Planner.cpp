#include <query/parse/Parser.h>
#include <query/planner/Planner.h>

namespace minisql {
namespace query {
namespace planner {

Planner::Planner(std::unique_ptr<QueryPlanner> qp,
                 std::unique_ptr<UpdatePlanner> up)
    : qp_(std::move(qp)), up_(std::move(up)) {}

std::unique_ptr<query::Plan> Planner::createQueryPlan(
    const std::string &qry, storage::tx::Transaction &tx) {
  auto parser = std::make_unique<parse::Parser>(qry);
  auto data = parser->query();
  return qp_->createPlan(*data, tx);
}

int32_t Planner::executeUpdate(const std::string &cmd,
                               storage::tx::Transaction &tx) {
  auto parser = std::make_unique<parse::Parser>(cmd);
  auto data = parser->updateCmd();
  switch (data->type()) {
    case parse::UpdateData::Type::Insert:
      return up_->executeInsert(*dynamic_cast<parse::InsertData *>(data.get()),
                                tx);
    case parse::UpdateData::Type::Modify:
      return up_->executeModify(*dynamic_cast<parse::ModifyData *>(data.get()),
                                tx);
    case parse::UpdateData::Type::Delete:
      return up_->executeDelete(*dynamic_cast<parse::DeleteData *>(data.get()),
                                tx);
    case parse::UpdateData::Type::CreateTable:
      return up_->executeCreateTable(
          *dynamic_cast<parse::CreateTableData *>(data.get()), tx);
    case parse::UpdateData::Type::CreateIndex:
      return up_->executeCreateIndex(
          *dynamic_cast<parse::CreateIndexData *>(data.get()), tx);
    case parse::UpdateData::Type::CreateView:
      return up_->executeCreateView(
          *dynamic_cast<parse::CreateViewData *>(data.get()), tx);
  }
  return 0;
}

}  // namespace planner
}  // namespace query
}  // namespace minisql