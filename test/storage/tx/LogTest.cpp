#include <common/Int32Constant.h>
#include <common/StringConstant.h>
#include <gtest/gtest.h>
#include <storage/file/FileMgr.h>
#include <storage/tx/log/BasicLogRecord.h>
#include <storage/tx/log/LogMgr.h>
#include <test/utils/RandomInputGenerator.h>
#include <test/utils/TestTable.h>
#include <boost/filesystem.hpp>

namespace minisql {
namespace storage {
namespace tx {
namespace log {

TEST(LogTest, AppendAndIterate0) {
  utils::initFileAndLog();

  utils::RandomInputGenerator rnd;

  auto s1 = rnd.string(128);
  auto s2 = rnd.string(128);

  std::vector<std::unique_ptr<common::Constant>> rec;
  rec.emplace_back(std::make_unique<common::StringConstant>(s1));
  rec.emplace_back(std::make_unique<common::StringConstant>(s2));

  auto lsn = LogMgr::get().append(rec);
  LogMgr::get().flush(lsn);

  auto iter = LogMgr::get().iterator();
  ASSERT_TRUE(iter->hasNext());

  auto basicRec = iter->next();
  ASSERT_EQ(basicRec->nextString(), s1);
  ASSERT_EQ(basicRec->nextString(), s2);
}

TEST(LogTest, AppendAndIterate1) {
  utils::initFileAndLog();

  std::vector<std::unique_ptr<common::Constant>> rec;
  rec.emplace_back(std::make_unique<common::Int32Constant>(105));
  rec.emplace_back(std::make_unique<common::Int32Constant>(106));
  rec.emplace_back(std::make_unique<common::StringConstant>("hello"));
  rec.emplace_back(std::make_unique<common::Int32Constant>(107));
  rec.emplace_back(std::make_unique<common::Int32Constant>(108));
  rec.emplace_back(std::make_unique<common::StringConstant>("world"));

  auto lsn = LogMgr::get().append(rec);
  LogMgr::get().flush(lsn);

  auto iter = LogMgr::get().iterator();
  ASSERT_TRUE(iter->hasNext());

  auto basicRec = iter->next();
  ASSERT_EQ(basicRec->nextInt32(), 105);
  ASSERT_EQ(basicRec->nextInt32(), 106);
  ASSERT_EQ(basicRec->nextString(), "hello");
  ASSERT_EQ(basicRec->nextInt32(), 107);
  ASSERT_EQ(basicRec->nextInt32(), 108);
  ASSERT_EQ(basicRec->nextString(), "world");
}

TEST(LogTest, AppendAndIterate2) {
  utils::initFileAndLog();

  utils::RandomInputGenerator rnd;
  auto repeatCount = static_cast<size_t>(rnd.number(256));
  size_t fieldsCount = 5;

  int32_t lsn = 0;
  std::vector<std::vector<std::string>> recList;
  recList.reserve(repeatCount);
  for (auto i = 0u; i < repeatCount; i++) {
    std::vector<std::string> sv;
    std::vector<std::unique_ptr<common::Constant>> rec;

    auto num = rnd.number(8192);
    sv.emplace_back(std::to_string(num));
    rec.emplace_back(std::make_unique<common::Int32Constant>(num));
    for (auto j = 0u; j < fieldsCount - 1; j++) {
      auto s = rnd.string(96);
      sv.emplace_back(s);
      rec.emplace_back(std::make_unique<common::StringConstant>(s));
    }
    lsn = LogMgr::get().append(rec);
    recList.emplace_back(sv);
  }

  LogMgr::get().flush(lsn);

  auto iter = LogMgr::get().iterator();
  ASSERT_TRUE(iter->hasNext());

  for (auto i = 0u; i < repeatCount; i++) {
    ASSERT_TRUE(iter->hasNext());
    auto basicRec = iter->next();
    ASSERT_EQ(basicRec->nextInt32(),
              std::stoi(recList[repeatCount - 1 - i][0]));
    for (auto j = 0u; j < fieldsCount - 1; j++) {
      ASSERT_EQ(basicRec->nextString(), recList[repeatCount - 1 - i][j + 1]);
    }
  }
}

}  // namespace log
}  // namespace tx
}  // namespace storage
}  // namespace minisql