#include <gtest/gtest.h>
#include <storage/file/Block.h>
#include <storage/file/FileMgr.h>
#include <test/utils/RandomInputGenerator.h>
#include <test/utils/TestTable.h>
#include <boost/filesystem.hpp>

namespace minisql {
namespace storage {
namespace file {

TEST(FileMgrTest, TestInitAndReadWrite) {
  utils::initFileMgr();

  // read write append
  utils::RandomInputGenerator rnd;
  const std::string fileName = "student.tbl";
  storage::file::Block blk(fileName, 0);

  const std::string s = rnd.string(128);
  auto ss = static_cast<int32_t>(s.size());
  FileMgr::get().write(blk, s.c_str(), ss);

  char r[1024];

  FileMgr::get().read(blk, r, ss);
  ASSERT_EQ(strncasecmp(s.c_str(), r, ss), 0);

  const std::string w = rnd.string(128);
  auto ws = static_cast<int32_t>(w.size());
  auto blk2 = FileMgr::get().append(fileName, w.c_str(), ws);

  FileMgr::get().read(*blk2, r, ws);
  ASSERT_EQ(strncasecmp(w.c_str(), r, ws), 0);
}

}  // namespace file
}  // namespace storage
}  // namespace minisql