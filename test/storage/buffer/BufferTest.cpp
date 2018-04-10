#include <gtest/gtest.h>
#include <init/MiniSQLInit.h>
#include <storage/buffer/Buffer.h>
#include <storage/buffer/BufferMgr.h>
#include <storage/file/Block.h>
#include <storage/tx/log/LogMgr.h>
#include <storage/tx/recovery/SetInt32Record.h>
#include <test/utils/TestTable.h>
#include <boost/filesystem.hpp>

namespace minisql {
namespace storage {

using buffer::BufferMgr;

TEST(BufferTest, WriteAndRead) {
  utils::initTempDB();

  auto blk = std::make_shared<file::Block>("student.tbl", 0);

  auto buff = BufferMgr::get().pin(blk);
  int32_t offset = 38;
  int32_t grad_year = buff->getInt32(offset);

  int32_t txNum = 1;
  tx::recovery::SetInt32Record rec(txNum, blk, offset, grad_year);
  auto lsn = rec.writeToLog();
  buff->setInt32(offset, grad_year + 1, txNum, lsn);
  BufferMgr::get().unpin(buff);

  buff = BufferMgr::get().pin(blk);
  ASSERT_EQ(buff->getInt32(offset), grad_year + 1);
  BufferMgr::get().unpin(buff);
}

}  // namespace storage
}  // namespace minisql