#include <glog/logging.h>
#include <gtest/gtest.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/Block.h>
#include <storage/record/RecordFile.h>
#include <storage/tx/Transaction.h>
#include <storage/tx/concurrency/LockAbortException.h>
#include <storage/tx/concurrency/LockTable.h>
#include <test/utils/TestTable.h>
#include <thread>

namespace minisql {
namespace storage {
namespace tx {

TEST(TransactionTest, RollbackTransaction) {
  utils::initTempDB();

  auto ti = utils::createTableInfo();

  Transaction tx;
  record::RecordFile rf(ti, tx);
  ASSERT_FALSE(rf.next());

  utils::RandomInputGenerator rnd;
  auto recordsCount = rnd.number(128);
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
  tx.commit();
  tx.rollback();

  record::RecordFile rf2(ti, tx);
  ASSERT_FALSE(rf2.next());
}

TEST(TransactionTest, ConcurrentRead) {
  utils::initTempDB();

  auto ti = utils::createTableInfo();

  Transaction tx;
  record::RecordFile rf(ti, tx);
  ASSERT_FALSE(rf.next());

  utils::RandomInputGenerator rnd;
  auto recordsCount = rnd.number(128);
  for (auto i = 0; i < recordsCount; i++) {
    rf.insert();
    ASSERT_NO_THROW(rf.setString(utils::fieldA, utils::fieldAVal));
    ASSERT_NO_THROW(rf.setInt32(utils::fieldB, utils::fieldBVal));
    ASSERT_NO_THROW(rf.setString(utils::fieldC, utils::fieldCVal));
  }
  rf.close();

  auto threadsCount = rnd.number(32);
  std::vector<std::thread> vt;
  for (auto i = 0; i < threadsCount; i++) {
    auto handler = [&ti, &recordsCount]() {
      storage::tx::Transaction tx2;
      record::RecordFile rf2(ti, tx2);
      rf2.beforeFirst();
      for (auto j = 0; j < recordsCount; j++) {
        ASSERT_TRUE(rf2.next());
        ASSERT_EQ(rf2.getString(utils::fieldA), utils::fieldAVal);
        ASSERT_EQ(rf2.getInt32(utils::fieldB), utils::fieldBVal);
        ASSERT_EQ(rf2.getString(utils::fieldC), utils::fieldCVal);
      }
      rf2.close();
      tx2.commit();
    };
    vt.emplace_back(std::thread(handler));
  }

  for (auto& t : vt) {
    t.join();
  }
}

TEST(TransactionTest, ConcurrentWrite) {
  utils::initFileAndLog();
  storage::buffer::BufferMgr::get().init(2, 200);

  concurrency::LockTable::get().init(200);

  auto ti = utils::createTableInfo();

  auto blk = std::make_shared<file::Block>("test.tbl", 0);

  std::mutex mFlag;
  bool t1Flag = false, t2Flag = false;
  int32_t offset = 0, t1Val = 100, t2Val = 200;

  auto t1Handler = [&blk, &mFlag, &t1Flag, &t2Flag, offset, t1Val]() {
    Transaction tx1;
    ASSERT_NO_THROW(tx1.pin(blk));
    LOG(INFO) << "t1 pin block " << blk->number();

    ASSERT_THROW(tx1.setInt32(blk, offset, t1Val),
                 concurrency::LockAbortException);

    t1Flag = true;
    t2Flag = false;

    LOG(INFO) << "t1 unpin";
    tx1.unpin(blk);
    tx1.commit();
  };

  auto t2Handler = [&blk, &mFlag, &t1Flag, &t2Flag, offset, t2Val]() {
    Transaction tx2;
    ASSERT_NO_THROW(tx2.pin(blk));
    LOG(INFO) << "t2 pin block " << blk->number();

    ASSERT_THROW(tx2.setInt32(blk, offset, t2Val),
                 concurrency::LockAbortException);

    t1Flag = false;
    t2Flag = true;

    LOG(INFO) << "t2 unpin";
    tx2.unpin(blk);
    tx2.commit();
  };

  std::thread t1(t1Handler);
  std::thread t2(t2Handler);
  t1.join();
  t2.join();
}

}  // namespace tx
}  // namespace storage
}  // namespace minisql