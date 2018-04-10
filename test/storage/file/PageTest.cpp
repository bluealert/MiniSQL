#include <gtest/gtest.h>
#include <boost/filesystem.hpp>

#include <storage/file/FileMgr.h>
#include <storage/file/Page.h>
#include <storage/record/RecordPageFormatter.h>
#include <storage/record/Schema.h>
#include <storage/record/TableInfo.h>

#include <test/utils/RandomInputGenerator.h>
#include <test/utils/TestTable.h>

namespace minisql {
namespace storage {
namespace file {

TEST(PageTest, AppendReadWrite) {
  utils::initFileMgr();

  auto ti = utils::createTableInfo();

  Page page;
  record::RecordPageFormatter fmtr(ti);
  fmtr.format(page);

  const std::string fileName = "student.tbl";
  auto blk = page.append(fileName);

  utils::RandomInputGenerator rnd;
  auto repeatCounts = rnd.number(1024);
  for (auto i = 0; i < repeatCounts; i++) {
    page.setString(ti->offset(utils::fieldA), utils::fieldAVal);
    page.setInt32(ti->offset(utils::fieldB), utils::fieldBVal);
    page.setString(ti->offset(utils::fieldC), utils::fieldCVal);

    page.write(*blk);
    page.read(*blk);

    ASSERT_EQ(utils::fieldAVal, page.getString(ti->offset(utils::fieldA)));
    ASSERT_EQ(utils::fieldBVal, page.getInt32(ti->offset(utils::fieldB)));
    ASSERT_EQ(utils::fieldCVal, page.getString(ti->offset(utils::fieldC)));
  }
}

}  // namespace file
}  // namespace storage
}  // namespace minisql