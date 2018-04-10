#include <glog/logging.h>
#include <gtest/gtest.h>
#include <init/MiniSQLInit.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferAbortException.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/Block.h>
#include <storage/file/FileMgr.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/SetInt32Record.h>
#include <test/utils/TestTable.h>
#include <boost/filesystem.hpp>
#include <thread>

namespace minisql {
namespace storage {

using buffer::BufferMgr;
using namespace std::chrono_literals;

TEST(BufferMgrTest, PinUnpinContention0) {
  utils::initFileMgr();

  BufferMgr::get().init(1, 100);

  std::thread t1([]() {
    auto blk = std::make_shared<file::Block>("student.tbl", 1);
    std::shared_ptr<buffer::Buffer> buff;
    ASSERT_NO_THROW(buff = BufferMgr::get().pin(blk));
    LOG(INFO) << "t1 pin block " << buff->block()->number();
    std::this_thread::sleep_for(150ms);
    LOG(INFO) << "t1 unpin";
    BufferMgr::get().unpin(buff);
  });

  std::thread t2([]() {
    std::this_thread::sleep_for(20ms);
    auto blk = std::make_shared<file::Block>("student.tbl", 2);
    ASSERT_THROW(BufferMgr::get().pin(blk), buffer::BufferAbortException);
    LOG(INFO) << "t2 pin block 2 failed";
  });

  t1.join();
  t2.join();
}

TEST(BufferMgrTest, PinUnpinContention1) {
  utils::initFileMgr();

  BufferMgr::get().init(1);

  std::vector<size_t> v;
  std::vector<std::thread> vt;
  size_t repeatCount = 10;
  for (size_t i = 0; i < repeatCount; i++) {
    auto handler = [&v, i]() {
      auto blk = std::make_shared<file::Block>("student.tbl", i);
      std::shared_ptr<buffer::Buffer> buff;
      ASSERT_NO_THROW(buff = BufferMgr::get().pin(blk));
      LOG(INFO) << "t" << i << " pin block " << buff->block()->number();
      v.push_back(i);
      std::this_thread::sleep_for(50ms);
      LOG(INFO) << "t" << i << " unpin block " << buff->block()->number();
      v.push_back(i);
      BufferMgr::get().unpin(buff);
    };
    vt.emplace_back(std::thread(handler));
  }

  for (auto& t : vt) {
    t.join();
  }

  for (size_t i = 0; i < v.size(); i += 2) {
    ASSERT_EQ(v[i], v[i + 1]);
  }
}

TEST(BufferMgrTest, PinUnpinContention2) {
  utils::initFileAndLog();

  BufferMgr::get().init(1);

  auto blk = std::make_shared<file::Block>("student.tbl", 0);

  std::mutex mFlag;
  bool t1Flag = false, t2Flag = false;
  int32_t offset = 0, t1Val = 100, t2Val = 200;

  auto t1Handler = [&blk, &mFlag, &t1Flag, &t2Flag, offset, t1Val]() {
    std::lock_guard<std::mutex> lk(mFlag);

    std::shared_ptr<buffer::Buffer> buff;
    ASSERT_NO_THROW(buff = BufferMgr::get().pin(blk));
    LOG(INFO) << "t1 pin block " << buff->block()->number();

    t1Flag = true;
    t2Flag = false;

    int32_t txNum = 1;
    tx::recovery::SetInt32Record rec(txNum, blk, offset, t1Val);
    auto lsn = rec.writeToLog();

    buff->setInt32(offset, t1Val, txNum, lsn);

    LOG(INFO) << "t1 unpin";
    BufferMgr::get().unpin(buff);
  };

  auto t2Handler = [&blk, &mFlag, &t1Flag, &t2Flag, offset, t2Val]() {
    std::lock_guard<std::mutex> lk(mFlag);

    std::shared_ptr<buffer::Buffer> buff;
    ASSERT_NO_THROW(buff = BufferMgr::get().pin(blk));
    LOG(INFO) << "t2 pin block " << buff->block()->number();

    t1Flag = false;
    t2Flag = true;

    int32_t txNum = 2;
    tx::recovery::SetInt32Record rec(txNum, blk, offset, t2Flag);
    auto lsn = rec.writeToLog();

    buff->setInt32(offset, t2Val, txNum, lsn);

    LOG(INFO) << "t2 unpin";
    BufferMgr::get().unpin(buff);
  };

  size_t repeatCount = 10;
  for (size_t i = 0; i < repeatCount; i++) {
    std::thread t1(t1Handler);
    std::thread t2(t2Handler);
    t1.join();
    t2.join();

    auto buff = BufferMgr::get().pin(blk);
    if (t1Flag) {
      ASSERT_FALSE(t2Flag);
      ASSERT_EQ(buff->getInt32(offset), t1Val);
    }
    if (t2Flag) {
      ASSERT_FALSE(t1Flag);
      ASSERT_EQ(buff->getInt32(offset), t2Val);
    }
    buff->unpin();
  }
}

}  // namespace storage
}  // namespace minisql