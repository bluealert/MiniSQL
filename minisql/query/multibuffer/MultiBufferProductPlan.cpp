#include <query/materialize/MaterializePlan.h>
#include <query/multibuffer/MultiBufferProductPlan.h>
#include <query/multibuffer/MultiBufferProductScan.h>
#include <query/scan/Scan.h>
#include <query/scan/UpdateScan.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace query {
namespace multibuffer {

using namespace storage;

MultiBufferProductPlan::MultiBufferProductPlan(const std::shared_ptr<Plan> &lhs,
                                               const std::shared_ptr<Plan> &rhs,
                                               tx::Transaction &tx)
    : lhs_(lhs), rhs_(rhs), tx_(tx) {
  sch_ = std::make_unique<record::Schema>();
  sch_->addAll(*lhs->schema());
  sch_->addAll(*rhs->schema());
}

std::unique_ptr<Scan> MultiBufferProductPlan::open() const {
  auto tt = copyRecordsFrom(rhs_);
  auto ti = tt->getTableInfo();
  auto leftScan = lhs_->open();
  return std::make_unique<MultiBufferProductScan>(std::move(leftScan), ti, tx_);
}

int32_t MultiBufferProductPlan::blocksAccessed() const {
  int32_t avail = buffer::BufferMgr::get().available();
  if (avail <= 0) {
    throw std::runtime_error(
        "MultiBufferProductPlan. the number of available buffers is 0");
  }
  materialize::MaterializePlan plan(rhs_, tx_);
  int32_t numChunks = plan.blocksAccessed() / avail;
  return rhs_->blocksAccessed() + (lhs_->blocksAccessed() * numChunks);
}

int32_t MultiBufferProductPlan::recordsOutput() const {
  return lhs_->recordsOutput() * rhs_->recordsOutput();
}

int32_t MultiBufferProductPlan::distinctValues(
    const std::string &fldName) const {
  if (lhs_->schema()->hasField(fldName)) {
    return lhs_->distinctValues(fldName);
  } else {
    return rhs_->distinctValues(fldName);
  }
}

std::shared_ptr<record::Schema> MultiBufferProductPlan::schema() const {
  return sch_;
}

std::unique_ptr<materialize::TempTable> MultiBufferProductPlan::copyRecordsFrom(
    std::shared_ptr<query::Plan> p) const {
  auto src = p->open();
  auto sch = p->schema();
  auto tt = std::make_unique<materialize::TempTable>(sch, tx_);
  auto dst = tt->open();
  while (src->next()) {
    dst->insert();
    for (auto &fldName : sch->fields()) {
      dst->setVal(fldName, *src->getVal(fldName));
    }
  }
  src->close();
  dst->close();
  return tt;
}

}  // namespace multibuffer
}  // namespace query
}  // namespace minisql