#include <query/plan/TablePlan.h>
#include <query/scan/TableScan.h>
#include <storage/metadata/MetadataMgr.h>
#include <storage/metadata/StatInfo.h>
#include <storage/record/RecordFile.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace query {

TablePlan::TablePlan(const std::string &tblName, storage::tx::Transaction &tx)
    : tx_(tx) {
  ti_ = storage::metadata::MetadataMgr::get().getTableInfo(tblName, tx);
  si_ = storage::metadata::MetadataMgr::get().getStatInfo(tblName, ti_, tx);
}

std::unique_ptr<Scan> TablePlan::open() const {
  return std::make_unique<TableScan>(ti_, tx_);
}

int32_t TablePlan::blocksAccessed() const { return si_->blocksAccessed(); }

int32_t TablePlan::recordsOutput() const { return si_->recordsOutput(); }

int32_t TablePlan::distinctValues(const std::string &fldName) const {
  return si_->distinctValues(fldName);
}

std::shared_ptr<storage::record::Schema> TablePlan::schema() const {
  return ti_->schema();
}

}  // namespace query
}  // namespace minisql