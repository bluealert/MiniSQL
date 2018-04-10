#pragma once

#include <memory>

namespace minisql {
namespace storage {

namespace tx {
class Transaction;
}

namespace metadata {

class TableMgr;

class ViewMgr {
 public:
  ViewMgr(bool isNew, const std::shared_ptr<TableMgr> &tblMgr,
          tx::Transaction &tx);
  void createView(const std::string &viewName, const std::string &viewDef,
                  tx::Transaction &tx);
  std::string getViewDef(const std::string &viewName, tx::Transaction &tx);

 private:
  std::shared_ptr<TableMgr> tblMgr_;
};

}  // namespace metadata
}  // namespace storage
}  // namespace minisql