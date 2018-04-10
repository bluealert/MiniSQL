#include <glog/logging.h>
#include <init/MiniSQLInit.h>
#include <query/planner/basic/BasicQueryPlanner.h>
#include <query/planner/basic/BasicUpdatePlanner.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/FileMgr.h>
#include <storage/metadata/MetadataMgr.h>
#include <storage/tx/Transaction.h>
#include <storage/tx/log/LogMgr.h>

namespace minisql {
namespace init {

static const int BUFFER_SIZE{8};

void MiniSQLInit::init(const std::string& dbPath) {
  google::InitGoogleLogging("MiniSQL");
  FLAGS_logtostderr = true;
  FLAGS_colorlogtostderr = true;

  auto isNew = initFileLogAndBuffer(dbPath);
  auto tx = std::make_unique<storage::tx::Transaction>();
  if (isNew) {
    LOG(INFO) << "Creating new database";
  } else {
    LOG(INFO) << "Recovering existing database";
    tx->recover();
  }
  storage::metadata::MetadataMgr::get().init(isNew, *tx);
  tx->commit();
}

bool MiniSQLInit::initFileLogAndBuffer(const std::string& dbPath) {
  const std::string LOG_FILE = "minisql.log";
  bool isNew = storage::file::FileMgr::get().init(dbPath);
  storage::tx::log::LogMgr::get().init(LOG_FILE);
  storage::buffer::BufferMgr::get().init(BUFFER_SIZE);
  return isNew;
}

std::unique_ptr<query::planner::Planner> MiniSQLInit::planner() {
  auto qp = std::make_unique<query::planner::basic::BasicQueryPlanner>();
  auto up = std::make_unique<query::planner::basic::BasicUpdatePlanner>();
  return std::make_unique<query::planner::Planner>(std::move(qp),
                                                   std::move(up));
}

}  // namespace init
}  // namespace minisql