#pragma once

#include <init/MiniSQLInit.h>
#include <storage/file/FileMgr.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/log/LogMgr.h>
#include <test/utils/RandomInputGenerator.h>
#include <boost/filesystem.hpp>
#include <string>

namespace utils {

std::string fieldA;
std::string fieldB;
std::string fieldC;

int32_t fieldBVal;
std::string fieldAVal;
std::string fieldCVal;

int32_t fieldASize;
int32_t fieldCSize;

void initFileMgr() {
  boost::system::error_code err;
  const std::string dbDir = "/tmp/minisql";
  if (boost::filesystem::exists(dbDir, err)) {
    boost::filesystem::remove_all(dbDir, err);
  }
  minisql::storage::file::FileMgr::get().init(dbDir);
}

void initFileAndLog() {
  boost::system::error_code err;
  const std::string dbDir = "/tmp/minisql";
  if (boost::filesystem::exists(dbDir, err)) {
    boost::filesystem::remove_all(dbDir, err);
  }
  const std::string LOG_FILE = "minisql.log";
  minisql::storage::file::FileMgr::get().init(dbDir);
  minisql::storage::tx::log::LogMgr::get().init(LOG_FILE);
}

void initTempDB() {
  boost::system::error_code err;
  const std::string dbPath = "/tmp/minisql";
  if (boost::filesystem::exists(dbPath, err)) {
    boost::filesystem::remove_all(dbPath, err);
  }
  minisql::init::MiniSQLInit::initFileLogAndBuffer(dbPath);
}

std::shared_ptr<minisql::storage::record::TableInfo> createTableInfo() {
  fieldA = "A";
  fieldB = "B";
  fieldC = "C";

  utils::RandomInputGenerator rnd;
  fieldAVal = rnd.string(128);
  fieldBVal = rnd.number(1 << 31);
  fieldCVal = rnd.string(128);

  fieldASize = static_cast<int32_t>(fieldAVal.size());
  fieldCSize = static_cast<int32_t>(fieldCVal.size());

  auto sch = std::make_shared<minisql::storage::record::Schema>();
  sch->addStringField(fieldA, fieldASize);
  sch->addInt32Field(fieldB);
  sch->addStringField(fieldC, fieldCSize);

  return std::make_shared<minisql::storage::record::TableInfo>("test", sch);
}

}  // namespace utils