#include <gtest/gtest.h>
#include <storage/record/RecordFile.h>
#include <storage/record/RecordPageFormatter.h>
#include <storage/record/TableInfo.h>
#include <storage/tx/Transaction.h>

#include <test/utils/RandomInputGenerator.h>
#include <test/utils/TestTable.h>

namespace minisql {
namespace storage {
namespace file {

TEST(RecordTest, EmptyRecordFileHasOnlyOneBlock) {
  utils::initTempDB();

  auto ti = utils::createTableInfo();

  storage::tx::Transaction tx;
  record::RecordFile rf(ti, tx);
  ASSERT_FALSE(rf.next());
}

TEST(RecordTest, SetGetData) {
  utils::initTempDB();

  auto ti = utils::createTableInfo();

  storage::tx::Transaction tx;
  record::RecordFile rf(ti, tx);
  ASSERT_FALSE(rf.next());

  utils::RandomInputGenerator rnd;
  auto recordsCount = rnd.number(1024);
  for (auto i = 0; i < recordsCount; i++) {
    rf.insert();
    ASSERT_NO_THROW(rf.setString(utils::fieldA, utils::fieldAVal));
    ASSERT_NO_THROW(rf.setInt32(utils::fieldB, utils::fieldBVal));
    ASSERT_NO_THROW(rf.setString(utils::fieldC, utils::fieldCVal));
  }

  rf.beforeFirst();
  for (auto i = 0; i < recordsCount; i++) {
    ASSERT_TRUE(rf.next());
    ASSERT_EQ(rf.getString(utils::fieldA), utils::fieldAVal);
    ASSERT_EQ(rf.getInt32(utils::fieldB), utils::fieldBVal);
    ASSERT_EQ(rf.getString(utils::fieldC), utils::fieldCVal);
  }
  rf.close();
}

}  // namespace file
}  // namespace storage
}  // namespace minisql