#include <storage/metadata/TableMgr.h>
#include <storage/metadata/ViewMgr.h>
#include <storage/record/RecordFile.h>
#include <storage/record/Schema.h>

namespace minisql {
namespace storage {
namespace metadata {

static const int32_t MAX_VIEWDEF{100};

ViewMgr::ViewMgr(bool isNew, const std::shared_ptr<TableMgr> &tblMgr,
                 tx::Transaction &tx)
    : tblMgr_(tblMgr) {
  if (isNew) {
    auto sch = std::make_shared<record::Schema>();
    sch->addStringField("viewname", TableMgr::MAX_NAME);
    sch->addStringField("viewdef", MAX_VIEWDEF);
    tblMgr_->createTable("viewcat", sch, tx);
  }
}

void ViewMgr::createView(const std::string &viewName,
                         const std::string &viewDef, tx::Transaction &tx) {
  auto ti = tblMgr_->getTableInfo("viewcat", tx);
  record::RecordFile rf(ti, tx);
  rf.insert();
  rf.setString("viewname", viewName);
  rf.setString("viewDef", viewDef);
  rf.close();
}

std::string ViewMgr::getViewDef(const std::string &viewName,
                                tx::Transaction &tx) {
  std::string result;
  auto ti = tblMgr_->getTableInfo("viewcat", tx);
  auto rf = std::make_unique<record::RecordFile>(ti, tx);
  while (rf->next()) {
    if (rf->getString("viewname") == viewName) {
      result = rf->getString("viewdef");
      break;
    }
  }
  rf->close();
  return result;
}

}  // namespace metadata
}  // namespace storage
}  // namespace minisql