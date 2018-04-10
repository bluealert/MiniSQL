#include <query/materialize/MaterializePlan.h>
#include <query/materialize/TempTable.h>
#include <storage/file/Page.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

namespace minisql {
namespace query {
namespace materialize {

using namespace storage;

MaterializePlan::MaterializePlan(const std::shared_ptr<Plan> &srcPlan,
                                 tx::Transaction &tx)
    : srcPlan_(srcPlan), tx_(tx) {}

std::unique_ptr<Scan> MaterializePlan::open() const {
  auto sch = srcPlan_->schema();
  auto temp = std::make_unique<TempTable>(sch, tx_);
  auto src = srcPlan_->open();
  auto dst = temp->open();
  while (src->next()) {
    dst->insert();
    for (auto &fldName : sch->fields()) {
      dst->setVal(fldName, *src->getVal(fldName));
    }
  }
  src->close();
  dst->beforeFirst();
  return dst;
}

int32_t MaterializePlan::blocksAccessed() const {
  auto ti = std::make_unique<record::TableInfo>("", srcPlan_->schema());
  auto rpb = (double)(file::Page::BLOCK_SIZE / ti->recordLength());
  return (int32_t)std::ceil(srcPlan_->recordsOutput() / rpb);
}

int32_t MaterializePlan::recordsOutput() const {
  return srcPlan_->recordsOutput();
}

int32_t MaterializePlan::distinctValues(const std::string &fldName) const {
  return srcPlan_->distinctValues(fldName);
}

std::shared_ptr<record::Schema> MaterializePlan::schema() const {
  return srcPlan_->schema();
}

}  // namespace materialize
}  // namespace query
}  // namespace minisql