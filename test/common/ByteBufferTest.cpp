#include <common/ByteBuffer.h>
#include <gtest/gtest.h>
#include <storage/file/Page.h>
#include <test/utils/RandomInputGenerator.h>

namespace minisql {
namespace common {

class ByteBufferTest : public ::testing::Test {
 public:
  static const int32_t BUF_SIZE{32};

  common::ByteBuffer buf_{BUF_SIZE};
};

TEST_F(ByteBufferTest, SetAndGetInt32) {
  utils::RandomInputGenerator rnd;
  size_t repeatCount = 256;
  for (size_t i = 0; i < repeatCount; i++) {
    auto num = rnd.number(1 << 31);
    buf_.setInt32(0, num);
    ASSERT_EQ(buf_.getInt32(0), num);
  }
}

TEST_F(ByteBufferTest, SetAndGetValues) {
  auto offset = 0;

  auto val = 205;
  buf_.setInt32(offset, val);
  ASSERT_EQ(buf_.getInt32(offset), val);

  auto str = std::string("hello");
  offset += storage::file::Page::INT32_SIZE;
  buf_.setString(offset, str);
  ASSERT_EQ(buf_.getString(offset), str);

  val = 11256;
  offset += storage::file::Page::STR_SIZE((int32_t)str.length());
  buf_.setInt32(offset, val);
  ASSERT_EQ(buf_.getInt32(offset), val);
}

TEST_F(ByteBufferTest, SetValueNoOverflow) {
  auto str = std::string("hellohellohellohellohello");
  auto offset = 0;
  ASSERT_NO_THROW(buf_.setString(offset, str));
}

TEST_F(ByteBufferTest, SetValueOverflow) {
  auto str = std::string("hellohellohellohellohellohello");
  auto offset = 0;
  ASSERT_THROW(buf_.setString(offset, str), std::overflow_error);
}

}  // namespace common
}  // namespace minisql